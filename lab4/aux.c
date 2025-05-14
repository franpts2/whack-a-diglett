#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include "aux.h"


#define MOUSE_IRQ 12

#define KBC_INPUT_BUFFER_FULL   BIT(1)
#define KBC_OUTPUT_BUFFER_FULL  BIT(0)
#define KBC_PARITY_ERROR        BIT(7)
#define KBC_TIMEOUT_ERROR       BIT(6)
#define MAX_ATTEMPTS            5
#define DELAY_US                20000  // 20ms


void (mouse_ih)(void) {
  uint8_t status;
  sys_inb(0x64, &status);  // check status first
  
  if (status & 0x01) {     // output buffer full
      uint8_t data;
      sys_inb(0x60, &data);
      
      if (!(status & 0xC0)) {  // no errors
          if (mouse_state.byte_count < 3) {
              mouse_state.bytes[mouse_state.byte_count++] = data;
              
              if (mouse_state.byte_count == 3) {
                  parse_packet();  // fill current struct
                  mouse_state.packet_ready = true;
                  mouse_state.packets_complete++;
                  mouse_state.byte_count = 0;
              }
          }
      }
  }
}

static int mouse_init() {
  // 1. Subscribe interrupts
  int hook_id = MOUSE_IRQ;
  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) != 0)
      return 1;
  
  // 2. Enable data reporting
  if (mouse_enable_data_reporting() != 0) {
      sys_irqrmpolicy(&hook_id);
      return 1;
  }
  
  // 3. Reset state
  memset(&mouse_state, 0, sizeof(mouse_state));
  return 0;
}

static void mouse_cleanup() {
  int hook_id = MOUSE_IRQ;

  // 1. Disable data reporting
  mouse_disable_data_reporting();
  
  // 2. Unsubscribe interrupts
  sys_irqrmpolicy(&hook_id);
}


int (mouse_enable_data_reporting)() {
  uint8_t response;
  
  // 1. send ENABLE_DATA_REPORTING command (0xF4)
  if (write_to_mouse(0xF4) != 0) {
      return 1; // error sending command
  }
  
  // 2. wait for ACK (0xFA)
  if (read_from_mouse(&response) != 0 || response != 0xFA) {
      return 1; // didn't get proper ACK
  }
  
  return 0; // success
}

int (mouse_disable_data_reporting)() {
  uint8_t response;
  
  // 1. send DISABLE_DATA_REPORTING command (0xF5)
  if (write_to_mouse(0xF5) != 0) {
      return 1; // error sending command
  }
  
  // 2. wait for ACK (0xFA)
  if (read_from_mouse(&response) != 0 || response != 0xFA) {
      return 1; // didn't get proper ACK
  }
  
  return 0; // success
}