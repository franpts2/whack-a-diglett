#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>
#include "controllers/timer/timer.h"
#include "controllers/kbdmouse/keyboard.h"
#include "controllers/kbdmouse/aux.h"

int(proj_main_loop)(int argc, char* argv[]) {
  int ipc_status;
  message msg;
  uint8_t timer_irq, kbd_irq, mouse_irq;


//Subscrever os interrupts do timer, keyboard e mouse

  if (timer_subscribe_int(&timer_irq)!=0) {
    printf("Failed to subscribe timer interrupt\n");
    return 1;
  }

  if (keyboard_subscribe_int(&kbd_irq)!=0) {
    printf("Failed to subscribe keyboard interrupt\n");
    timer_unsubscribe_int();
    return 1;
  }

  if (mouse_subscribe_int(&mouse_irq)!=0) {
    printf("Failed to subscribe mouse interrupt\n");
    keyboard_unsubscribe_int();
    timer_unsubscribe_int();
    return 1;
  }


  // Loop da lógica do jogo
  int keep_running = 1;
  while (keep_running) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0) {
      printf("driver_receive failed\n");
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & timer_irq) {
            printf("Timer interrupt!\n");
          }
          if (msg.m_notify.interrupts & kbd_irq) {
            kbc_ih();
            printf("Keyboard interrupt!\n");
            extern uint8_t scancode;
            // ESC acaba o loop
            if (scancode == 0x81) keep_running = 0;
          }
          if (msg.m_notify.interrupts & mouse_irq) {
            mouse_ih();
            printf("Mouse interrupt!\n");
          }
          break;
        default:
          break;
      }
    }
  }

  // Unsubscribe dos interrupts
  mouse_unsubscribe_int();
  keyboard_unsubscribe_int();
  timer_unsubscribe_int();

  return 0;
}

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  if (lcf_start(argc, argv))
    return 1;

  lcf_cleanup();
  return 0;
}
