#include "game.h"
#include "../controllers/kbdmouse/keyboard.h"
#include "modes/menu.h"
#include "modes/playing.h"
#include <lcom/lcf.h>
#include <stdio.h>

GameMode current_mode = MODE_MENU;
GameMode prev_mode = -1;
int prev_selected = -1;

int game_main_loop(void) {
  // Subscrever interrupções do teclado
  uint8_t kbd_irq;
  if (keyboard_subscribe_int(&kbd_irq) != 0) {
    printf("Failed to subscribe keyboard interrupt\n");
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
              // handle input based on current game mode
              switch (current_mode) {
                case MODE_MENU:
                  menu_handle_input(scancode);
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
          break;
        default:
          break;
      }
    }

    // check for mode changes and initialize 
    if (current_mode != prev_mode) {
      switch (current_mode) {
        case MODE_MENU:
          menu_init();
          prev_selected = -1;
          break;
        case MODE_PLAYING:
          playing_init();
          break;
        case MODE_INSTRUCTIONS:
          // instructions_init();
          break;
        default:
          break;
      }
      prev_mode = current_mode;
    }

    extern int selected;
    if (current_mode == MODE_MENU && selected != prev_selected) {
      menu_update_selection();
      prev_selected = selected;
    }
    else if (current_mode == MODE_PLAYING) {
      playing_update(); // update game state
    }
  }

  keyboard_unsubscribe_int();

  return 0;
}
