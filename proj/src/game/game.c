#include "game.h"
#include "../controllers/kbdmouse/keyboard.h"
#include "cursor/cursor.h"
#include "modes/menu.h"
#include <lcom/lcf.h>
#include <stdio.h>

#include "../controllers/kbdmouse/aux.h"
#include "../controllers/video/video.h"

GameMode current_mode = MODE_MENU;
GameMode prev_mode = -1;
int prev_selected = -1;

Cursor *g_cursor = NULL;

int game_main_loop(void) {
  // Initialize cursor
  g_cursor = cursor_init();
  if (g_cursor == NULL) {
    printf("Failed to initialize cursor\n");
    return 1;
  }

  uint8_t kbd_irq;
  if (keyboard_subscribe_int(&kbd_irq) != 0) {
    printf("Failed to subscribe keyboard interrupt\n");
    cursor_destroy(g_cursor);
    return 1;
  }

  uint8_t mouse_irq;
  if (mouse_subscribe_int(&mouse_irq) != 0) {
    printf("Failed to subscribe mouse interrupt\n");
    keyboard_unsubscribe_int();
    cursor_destroy(g_cursor);
    return 1;
  }

  if (my_mouse_enable_data_reporting() != 0) {
    printf("Failed to enable mouse data reporting\n");
    mouse_unsubscribe_int();
    keyboard_unsubscribe_int();
    cursor_destroy(g_cursor);
    return 1;
  }

  int running = 1;
  int ipc_status;
  message msg;

  while (running) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0) {
      printf("driver_receive failed\n");
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & kbd_irq) {
            kbc_ih();
            extern uint8_t scancode;
            if ((scancode & 0x80) == 0) {
              menu_handle_input(scancode);
            }
            if (scancode == 0x81)
              running = 0; // ESC para parar
          }

          // handle mouse ints
          if (msg.m_notify.interrupts & mouse_irq) {
            mouse_ih();
            mouse_collect_packet_byte();

            if (byte_index == 3) {
              assemble_mouse_packet();

              // update cursor position
              if (g_cursor != NULL) {
                // current cursor position
                int new_x = g_cursor->x + mouse_packet.delta_x;
                int new_y = g_cursor->y - mouse_packet.delta_y; // Y is inverted

                // keep inside screen (800x600)
                if (new_x < 0)
                  new_x = 0;
                if (new_x > 800 - g_cursor->width)
                  new_x = 800 - g_cursor->width;
                if (new_y < 0)
                  new_y = 0;
                if (new_y > 600 - g_cursor->height)
                  new_y = 600 - g_cursor->height;

                cursor_set_position(g_cursor, new_x, new_y);
              }

              // Reset byte index for next packet
              byte_index = 0;
            }
          }
          break;
        default:
          break;
      }
    }

    // check if changed to new mode
    if (current_mode != prev_mode) {
      if (current_mode == MODE_MENU) {
        prev_mode = MODE_MENU;
        prev_selected = -1;

        // prepare static background only once
        set_drawing_to_static();

        // clear static buffer and draw static content
        unsigned int bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
        unsigned int buffer_size = m_info.XResolution * m_info.YResolution * bytes_per_pixel;
        memset(static_buffer, 0, buffer_size);

        draw_menu_bg_and_buttons();
        set_drawing_to_back();
      }
    }

    copy_static_to_back(); // also sets current_drawing_buffer to back_buffer

    if (current_mode == MODE_MENU) {
      // update selection if needed
      extern int selected;
      if (selected != prev_selected) {
        prev_selected = selected;
      }
      draw_menu_selection(); // always draw selection (dynamic)

      // draw cursor on top of everything
      if (g_cursor != NULL) {
        cursor_draw(g_cursor);
      }
    }

    swap_buffers();
  }

  // cleanup
  mouse_disable_data_reporting();
  mouse_unsubscribe_int();
  keyboard_unsubscribe_int();
  cursor_destroy(g_cursor);
  destroy_buffers();

  return 0;
}
