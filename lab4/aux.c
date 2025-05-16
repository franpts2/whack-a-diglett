#include <lcom/lcf.h>

#include "aux.h"
#include <stdint.h>
#include <stdio.h>

#include <minix/sysutil.h>

int hook_id_mouse = 3;
uint8_t current_byte;

struct packet mouse_packet;
uint8_t byte_index = 0;
uint8_t mouse_bytes[3];


int(write_to_mouse)(uint8_t command) {
  uint8_t mouse_response;

  for (uint8_t attempts = MAX_ATTEMPTS; attempts > 0; attempts--) {
    if (write_KBC_command(KBC_IN_COMMAND, WRITE_BYTE_MOUSE))
      return 1;
    if (write_KBC_command(KBC_OUT_COMMAND, command))
      return 1;
    tickdelay(micros_to_ticks(WAIT_TIME));
    if (util_sys_inb(KBC_OUT_COMMAND, &mouse_response))
      return 1;
    if (mouse_response == ACK)
      return 0;
  
    // only continue loop if we didn't get ACK
    if (mouse_response == ACK)
      break;
  }

  return 1;
}

int(read_from_mouse)(uint8_t *output) {
  if (output == NULL)
    return 1;

  // Send the Read Data command to mouse
  if (write_to_mouse(MOUSE_READ_DATA) != 0)
    return 1;

  // Wait for the mouse to send the data (ACK has already been handled by write_to_mouse)
  tickdelay(micros_to_ticks(WAIT_TIME));

  // Read the actual data byte from the mouse
  if (read_KBC_output(KBC_OUT_COMMAND, output, 1) != 0)
    return 1;

  return 0;
}

void(assemble_mouse_packet)() {
  for (int i = 0; i < 3; i++) {
    mouse_packet.bytes[i] = mouse_bytes[i];
  }

  mouse_packet.lb = mouse_bytes[0] & MOUSE_LB;
  mouse_packet.mb = mouse_bytes[0] & MOUSE_MB;
  mouse_packet.rb = mouse_bytes[0] & MOUSE_RB;

  mouse_packet.x_ov = mouse_bytes[0] & MOUSE_X_OVERFLOW;
  mouse_packet.y_ov = mouse_bytes[0] & MOUSE_Y_OVERFLOW;
  
  // check if the sign bit is set (negative value)
  bool x_negative = mouse_bytes[0] & MOUSE_X_SIGNAL;
  // if negative, extend the sign (2's complement) by setting upper 8 bits to 1
  // otherwise use the raw value
  mouse_packet.delta_x = x_negative ? (0xFF00 | mouse_bytes[1]) : mouse_bytes[1];
  
  bool y_negative = mouse_bytes[0] & MOUSE_Y_SIGNAL;
  mouse_packet.delta_y = y_negative ? (0xFF00 | mouse_bytes[2]) : mouse_bytes[2];
}

void(mouse_ih)() {
  if (read_KBC_output(KBC_OUT_COMMAND, &current_byte, 1))
    printf("couldn't read byte from mouse\n");
}

int(mouse_init)(uint8_t *mouse_mask) {
  if (mouse_subscribe_int(mouse_mask) != 0)
    return 1;

  if (write_to_mouse(ENABLE_DATA_REPORTING) != 0)
    return 1;

  return 0;
}

int(mouse_cleanup)() {
  if (write_to_mouse(DISABLE_DATA_REPORTING) != 0)
    return 1;

  if (mouse_unsubscribe_int() != 0)
    return 1;

  return 0;
}

int (my_mouse_enable_data_reporting)() {
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

int(util_sys_inb)(int port, uint8_t *value) {
  if (value == NULL)
    return 1;

  uint32_t val;
  if (sys_inb(port, &val) != 0)
    return 1;

  *value = (uint8_t) (val & 0xFF);
  return 0;
}

int(mouse_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL)
    return 1;
  *bit_no = BIT(hook_id_mouse);
  return sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_mouse);
}

int(mouse_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id_mouse);
}

int(write_KBC_command)(uint8_t port, uint8_t commandByte) {
  uint8_t status;
  uint8_t attemps = MAX_ATTEMPTS;

  while (attemps > 0) {
    if (read_KBC_status(&status) != 0) {
      printf("tatus not available!\n");
      return 1;
    }

    if (!(status & IN_BUF_FULL)) {
      if (sys_outb(port, commandByte) != 0) {
        printf("ould not write commandByte!\n");
        return 1;
      }
      return 0;
    }
    tickdelay(micros_to_ticks(WAIT_TIME));
    attemps--;
  }
  return 1;
}

int(read_KBC_status)(uint8_t *status) {
  return util_sys_inb(KBC_STATUS_REG, status);
}


int(read_KBC_output)(uint8_t port, uint8_t *output, uint8_t mouse) {
  uint8_t status;
  uint8_t attemps = MAX_ATTEMPTS;

  while (attemps > 0) {
    if (read_KBC_status(&status) != 0) {
      printf("status not available\n");
      return 1;
    }

    if ((status & BIT(0)) != 0) {
      if (util_sys_inb(port, output) != 0) {
        printf("couldn't read output\n");
        return 1;
      }
      if ((status & PARITY_ERROR)) {
        printf("parity error\n");
        return 1;
      }
      if ((status & TIMEOUT_ERROR)) {
        printf("timeout error!\n");
        return 1;
      }
      if (mouse && !(status & BIT(5))) {
        printf("mouse output not found\n");
        return 1;
      }
      if (!mouse && (status & BIT(5))) {
        printf("keyboard output not found\n");
        return 1;
      }
      return 0;
    }
    tickdelay(micros_to_ticks(20000));
    attemps--;
  }
  return 1;
}

void(mouse_collect_packet_byte)() {

  if (byte_index == 0 && (current_byte & FIRST_BYTE)) {
    mouse_bytes[byte_index] = current_byte;
    byte_index++;
  }

  else if (byte_index > 0) {
    mouse_bytes[byte_index] = current_byte;
    byte_index++;
  }
}
