#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>
#include "keyboard.h"

#include "i8042.h"

extern uint32_t counter_KBC;
extern int counter_TIMER;
extern uint8_t scancode;
unsigned char bytes[2];

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  uint8_t irq_set;
  int ipc_status, r;
  message msg;

  // subscribe keyboard interrupts
  if (keyboard_subscribe_int(&irq_set) != 0) return 1;

  // terminate when ESC break code is received
  while (scancode != BREAK_ESC) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            kbc_ih();
            bytes[0] = scancode;
            kbd_print_scancode((scancode&BIT(7))==0, 1, bytes);
          }
          break;

        default:
          break;
      }
    }
  }

  // unsubscribe keyboard interrupts
  if (keyboard_unsubscribe_int() != 0) return 1;

  return 0;
}

int(kbd_test_poll)() {
  uint8_t commandByte;

  // disable keyboard interrupts
  if (read_KBC_output(KBC_READ_CMD, &commandByte, 0) != 0) return 1;
  commandByte &= ~ENABLE_INT;
  if (write_KBC_command(KBC_WRITE_CMD, commandByte) != 0) return 1;

  while (scancode != BREAK_ESC) {
    uint8_t status;
    if (read_KBC_status(&status) != 0) continue;

    if ((status & FULL_OUT_BUFFER) && !(status & FULL_IN_BUFFER)) {
      if (read_KBC_output(KBC_OUT_CMD, &scancode, 0) != 0) continue;

      if (status & (TIMEOUT_ERROR | PARITY_ERROR)) {
        printf("Error: Timeout or Parity error\n");
        continue;
      }

      bytes[0] = scancode;
      kbd_print_scancode((scancode&BIT(7))==0, 1, bytes);
    }
  }

  // reenable keyboard interrupts
  commandByte |= ENABLE_INT;
  if (write_KBC_command(KBC_WRITE_CMD, commandByte) != 0) return 1;

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  int ipc_status;
  uint8_t irq_set_TIMER, irq_set_KBC;
  message msg;
  
  int ipc_status;

  // subscribe both keyboard and timer interrupts
  if(keyboard_subscribe_int(&irq_set_KBC) != 0) return 1;
  if(timer_subscribe_int(&irq_set_TIMER) != 0) return 1;

  while (scancode != BREAK_ESC) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set_KBC) {
            kbc_ih();
            bytes[0] = scancode;
            kbd_print_scancode((scancode&BIT(7))==0, 1, bytes);
          }
          if (msg.m_notify.interrupts & irq_set_TIMER) {
            timer_int_handler();
            if (counter_TIMER == n) {
              break;
            }
          }
          break;

        default:
          break;
      }
    }
  }

  // unsubscribe both keyboard and timer interrupts
  if (keyboard_unsubscribe_int()!=0) return 1;
  if (timer_unsubscribe_int()!=0) return 1;
  
  return 0;

  /*

  1. subscribe both keyboard and timer interrupts 

  2. interrupt handling looop
  - check both timer and keyboard interrupts in the same loop
  - for timer interrupts:
    - track idle time since last keypress
    - exit if idle time exceeds specified duration
  - for keyboard interrupts:
    - process scancodes as in kbd_test_scan()
    - reset idle timer on each keypress
    - exit on ESC break code (0x81)

  3. unsubscribe both keyboard and timer interrupts

  */
}

