// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "aux.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(mouse_test_packet)(uint32_t cnt) {

  int ipc_status;
  message msg;
  uint8_t mouse_mask;

  if (mouse_init(&mouse_mask) != 0)
    return 1;

  while (cnt > 0) {

    if (driver_receive(ANY, &msg, &ipc_status) != 0) {
      printf("error");
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & mouse_mask) {
            mouse_ih();
            mouse_collect_packet_byte();
            if (byte_index == 3) {
              assemble_mouse_packet();
              mouse_print_packet(&mouse_packet);
              byte_index = 0;
              cnt--;
            }
          }
          break;
      }
    }
  }

  if (mouse_cleanup() != 0)
    return 1;

  return 0;
}

int(mouse_test_async)(uint8_t idle_time) {
  int ipc_status;
  message msg;
  uint8_t seconds = 0;
  uint8_t mouse_mask = 0, timer_mask = 0;
  uint32_t timer_frequency = sys_hz();
  extern int counter; // Access the counter variable from timer.c

  // Initialize mouse and subscribe to timer
  if (mouse_init(&mouse_mask) != 0) return 1;
  if (timer_subscribe_int(&timer_mask) != 0) {
    mouse_cleanup();
    return 1;
  }

  while (seconds < idle_time) { // Terminate after idle_time without mouse packets

    if (driver_receive(ANY, &msg, &ipc_status) != 0) {
      printf("Error receiving message\n");
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:

          if (msg.m_notify.interrupts & timer_mask) { // Timer interrupt
            timer_int_handler();
            if (counter % timer_frequency == 0) seconds++;
          }

          if (msg.m_notify.interrupts & mouse_mask) { // Mouse interrupt
            mouse_ih();
            mouse_collect_packet_byte();

            if (byte_index == 3) {
              assemble_mouse_packet();
              mouse_print_packet(&mouse_packet);
              byte_index = 0;
            }

            // Reset idle time counters whenever mouse data is received
            seconds = 0;
            counter = 0;
          }
          break;
      }
    }
  }

  // Cleanup: unsubscribe timer and mouse interrupts
  if (timer_unsubscribe_int() != 0) {
    mouse_cleanup();
    return 1;
  }

  if (mouse_cleanup() != 0) return 1;

  return 0;
}

int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  // initialize state variables
  gesture_state_t state = INIT;
  int16_t x_displacement_up = 0;
  int16_t y_displacement_up = 0;
  int16_t x_displacement_down = 0;
  int16_t y_displacement_down = 0;

  int ipc_status;
  message msg;
  uint8_t mouse_mask;

  if (mouse_init(&mouse_mask) != 0)
    return 1;

  bool running = true;
  while (running) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0) {
      printf("Error receiving message\n");
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & mouse_mask) {
            mouse_ih();
            mouse_collect_packet_byte();

            if (byte_index == 3) {
              assemble_mouse_packet();
              mouse_print_packet(&mouse_packet);
              byte_index = 0;

              // State machine for gesture detection
              switch (state) {
                case INIT:
                  state = process_init_state(&mouse_packet);
                  // if transitioning to DRAW_UP, reset displacement counters
                  if (state == DRAW_UP) {
                    x_displacement_up = 0;
                    y_displacement_up = 0;
                  }
                  break;

                case DRAW_UP:
                  state = process_draw_up_state(&mouse_packet, &x_displacement_up,
                                                &y_displacement_up, x_len, tolerance);
                  break;

                case VERTEX:
                  state = process_vertex_state(&mouse_packet, tolerance);
                  // if transitioning to DRAW_DOWN, reset displacement counters
                  if (state == DRAW_DOWN) {
                    x_displacement_down = 0;
                    y_displacement_down = 0;
                  }
                  break;

                case DRAW_DOWN:
                  state = process_draw_down_state(&mouse_packet, &x_displacement_down,
                                                  &y_displacement_down, x_len, tolerance);
                  // if gesture is complete, exit the loop
                  if (state == COMPLETE) {
                    running = false;
                  }
                  break;

                default:
                  break;
              }
            }
          }
          break;
      }
    }
  }

  if (mouse_cleanup() != 0)
    return 1;

  return 0;
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt) {
  /* This year you need not implement this. */
  printf("%s(%u, %u): under construction\n", __func__, period, cnt);
  return 1;
}
