#include "game.h"
#include "../controllers/kbdmouse/keyboard.h"
#include "../controllers/timer/timer.h"
#include "background.h"
#include "cursor/cursor.h"
#include "modes/choose_mode.h"
#include "modes/menu.h"
#include "modes/playing-modes/playing_kbd.h"
#include "modes/playing-modes/playing_mouse.h"
#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdio.h>

#include "../controllers/kbdmouse/aux.h"
#include "../controllers/video/video.h"
#include "../fonts/testfont.h"

unsigned int bytes_per_pixel;
unsigned int buffer_size;

GameMode current_mode = MODE_MENU;
GameMode prev_mode = -1;
int prev_selected = -1;
int running = 1;

Cursor *g_cursor = NULL;
extern int counter; // timer counter

// frame rate control constants and variables
#define FRAME_RATE 60
#define TICKS_PER_FRAME (sys_hz() / FRAME_RATE)
uint32_t frame_timer = 0;
bool render_frame = false;

// Add a static variable to track previous mouse button state
static bool prev_left_button_state = false;

int game_main_loop(void) {
  if (background_init() != 0) {
    printf("Failed to initialize background\n");
    return 1;
  }

  g_cursor = cursor_init();
  if (g_cursor == NULL) {
    printf("Failed to initialize cursor\n");
    background_destroy();
    return 1;
  }

  uint8_t kbd_irq;
  if (keyboard_subscribe_int(&kbd_irq) != 0) {
    printf("Failed to subscribe keyboard interrupt\n");
    cursor_destroy(g_cursor);
    background_destroy();
    return 1;
  }

  uint8_t mouse_irq;
  if (mouse_subscribe_int(&mouse_irq) != 0) {
    printf("Failed to subscribe mouse interrupt\n");
    keyboard_unsubscribe_int();
    cursor_destroy(g_cursor);
    background_destroy();
    return 1;
  }

  if (my_mouse_enable_data_reporting() != 0) {
    printf("Failed to enable mouse data reporting\n");
    mouse_unsubscribe_int();
    keyboard_unsubscribe_int();
    cursor_destroy(g_cursor);
    background_destroy();
    return 1;
  }

  uint8_t timer_irq;
  if (timer_subscribe_int(&timer_irq) != 0) {
    printf("Failed to subscribe timer interrupt\n");
    mouse_disable_data_reporting();
    mouse_unsubscribe_int();
    keyboard_unsubscribe_int();
    cursor_destroy(g_cursor);
    background_destroy();
    return 1;
  }

  // timer frequency to 60Hz (for smoother animations)
  timer_set_frequency(0, 60);

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
          if (msg.m_notify.interrupts & timer_irq) {
            timer_int_handler();
            frame_timer++;

            if (frame_timer >= TICKS_PER_FRAME) {
              // Only set render_frame to true if we're not in a mode transition
              if (prev_mode == current_mode) {
                render_frame = true;

                if (current_mode == MODE_PLAYING) {
                  if (mode_selected == 0) { // Keyboard mode
                    playing_kbd_update();
                  }
                  else { // Mouse mode
                    playing_mouse_update();
                  }

                  swap_buffers();
                }
              }
              frame_timer = 0;
            }
          }

          if (msg.m_notify.interrupts & kbd_irq) {
            kbc_ih();
            extern uint8_t scancode;
            if ((scancode & 0x80) == 0) {
              // handle input based on current game mode
              switch (current_mode) {
                case MODE_MENU:
                  menu_handle_input(scancode);
                  break;
                case MODE_CHOOSE_MODE:
                  choose_mode_handle_input(scancode);
                  break;
                case MODE_PLAYING:
                  playing_handle_input(scancode);
                  break;
                default:
                  break;
              }
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

              // update cursor position if in menu or choose mode
              if (g_cursor != NULL && (current_mode == MODE_MENU || current_mode == MODE_CHOOSE_MODE)) {
                cursor_handle_mouse_packet(g_cursor, &mouse_packet);

                bool left_button_pressed = mouse_packet.lb;
                bool left_button_clicked = left_button_pressed && !prev_left_button_state;

                prev_left_button_state = left_button_pressed;

                // Handle mouse input based on current mode
                if (current_mode == MODE_MENU) {
                  menu_handle_mouse(g_cursor->x, g_cursor->y, left_button_clicked);
                }
                else if (current_mode == MODE_CHOOSE_MODE) {
                  choose_mode_handle_mouse(g_cursor->x, g_cursor->y, left_button_clicked);
                }
              }

              // reset byte index for next packet
              byte_index = 0;
            }
          }
          break;
        default:
          break;
      }
    }

    // check for mode changes and initialize - only once per mode change
    if (current_mode != prev_mode) {
      // store the current mode
      GameMode new_mode = current_mode;

      switch (new_mode) {
        case MODE_MENU:
          // reset menu and selection
          menu_init();

          set_drawing_to_static();

          // clear static buffer and draw static content
          bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
          buffer_size = m_info.XResolution * m_info.YResolution * bytes_per_pixel;
          memset(static_buffer, 0, buffer_size);

          draw_menu_bg_and_buttons();
          set_drawing_to_back();

          // force a render frame to ensure selection is drawn
          render_frame = true;
          break;

        case MODE_CHOOSE_MODE:
          choose_mode_init();

          set_drawing_to_static();

          bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
          buffer_size = m_info.XResolution * m_info.YResolution * bytes_per_pixel;
          memset(static_buffer, 0, buffer_size);

          draw_choose_mode_bg_and_buttons();
          set_drawing_to_back();

          render_frame = true;
          break;

        case MODE_PLAYING:
          // clear the display completely
          bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
          buffer_size = m_info.XResolution * m_info.YResolution * bytes_per_pixel;

          // reset both buffers to avoid showing previous menu content
          memset(static_buffer, 0, buffer_size);
          memset(back_buffer, 0, buffer_size);

          set_drawing_to_back();

          render_frame = true;
          swap_buffers();
          break;
        case MODE_INSTRUCTIONS:
          // instructions_init();
          break;
        default:
          break;
      }
      prev_mode = current_mode;
    }

    // render on mouse movement for responsiveness, use timer for animations, track recent movement for smoothness
    static bool mouse_moved_recently = false;
    bool mouse_moving_now = (mouse_packet.delta_x != 0 || mouse_packet.delta_y != 0);

    if (mouse_moving_now) {
      // Process mouse movement in menu or choose mode
      if (current_mode == MODE_MENU) {
        copy_static_to_back();
        draw_menu_selection();
        if (g_cursor != NULL) {
          cursor_draw(g_cursor);
        }
        swap_buffers(); // swap immediately for low latency
        mouse_moved_recently = true;
        render_frame = false; // reset frame timer
      }
      else if (current_mode == MODE_CHOOSE_MODE) {
        copy_static_to_back();
        choose_mode_update_selection();
        if (g_cursor != NULL) {
          cursor_draw(g_cursor);
        }
        swap_buffers();
        mouse_moved_recently = true;
        render_frame = false;
      }
    }
    // regular rendering (not actively moving mouse)
    else if (render_frame || mouse_moved_recently) {
      if (current_mode == MODE_MENU) {
        copy_static_to_back();
        draw_menu_selection();

        // draw cursor on top
        if (g_cursor != NULL) {
          cursor_draw(g_cursor);
        }

        swap_buffers();
      }
      else if (current_mode == MODE_CHOOSE_MODE) {
        copy_static_to_back();
        choose_mode_update_selection();

        // draw cursor on top
        if (g_cursor != NULL) {
          cursor_draw(g_cursor);
        }

        swap_buffers();
      }

      render_frame = false;

      // reset mouse movement flag after a few frames to avoid unnecessary rendering
      if (mouse_moved_recently && !mouse_moving_now) {
        static int post_movement_frames = 0;
        if (++post_movement_frames >= 2) {
          mouse_moved_recently = false;
          post_movement_frames = 0;
        }
      }
    }
  }

  // cleanup
  mouse_disable_data_reporting();
  mouse_unsubscribe_int();
  keyboard_unsubscribe_int();
  timer_unsubscribe_int();
  background_destroy();
  cursor_destroy(g_cursor);
  destroy_buffers();

  return 0;
}
