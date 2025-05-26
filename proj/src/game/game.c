#include "game.h"
#include "../controllers/kbdmouse/keyboard.h"
#include "cursor/cursor.h"
#include "modes/menu.h"
#include <lcom/lcf.h>
#include <stdio.h>

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
          break;
        default:
          break;
      }
    }

    if (current_mode == MODE_MENU && prev_mode != MODE_MENU) {
      menu_init();
      prev_mode = MODE_MENU;
      prev_selected = -1;
    }

    extern int selected;
    if (current_mode == MODE_MENU && selected != prev_selected) {
      menu_update_selection();
      prev_selected = selected;
    }

    if (g_cursor != NULL) {
      cursor_draw(g_cursor);
    }
  }

  keyboard_unsubscribe_int();
  cursor_destroy(g_cursor);

  return 0;
}
