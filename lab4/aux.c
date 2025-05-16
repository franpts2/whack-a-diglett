#include <lcom/lcf.h>

#include "aux.h"
#include <stdint.h>
#include <stdio.h>

static int write_to_mouse(uint8_t command) {
  uint32_t status;
  unsigned int attempts = 0;

  // wait for KBC input buffer to be empty
  while (attempts < MAX_ATTEMPTS) {
    if (sys_inb(KBC_STATUS_REG, &status) != 0)
      return 1;

    if ((status & KBC_INPUT_BUFFER_FULL) == 0) {
      // Write mouse command prefix (0xD4)
      if (sys_outb(KBC_CMD_REG, 0xD4) != 0)
        return 1;

      // Write actual command
      if (sys_outb(KBC_DATA_REG, command) != 0)
        return 1;
      return 0;
    }

    tickdelay(micros_to_ticks(DELAY_US));
    attempts++;
  }

  return 1; // timeout
}

static int read_from_mouse(uint8_t *data) {
  uint32_t status;
  uint32_t temp_data;
  unsigned int attempts = 0;

  // wait for data in output buffer
  while (attempts < MAX_ATTEMPTS) {
    if (sys_inb(KBC_STATUS_REG, &status) != 0)
      return 1;

    if (status & KBC_OUTPUT_BUFFER_FULL) {
      if (status & (KBC_PARITY_ERROR | KBC_TIMEOUT_ERROR)) {
        return 1; // Error in communication
      }
      // Read into temp_data and cast to uint8_t when storing
      if (sys_inb(KBC_DATA_REG, &temp_data) != 0)
        return 1;
        
      *data = (uint8_t)temp_data;
      return 0; // Success
    }

    tickdelay(micros_to_ticks(DELAY_US));
    attempts++;
  }

  return 1; // timeout
}

static void parse_packet() {
  struct packet *packet = &mouse_state.current;

  // copy raw bytes from the mouse_state to the packet
  packet->bytes[0] = mouse_state.bytes[0];
  packet->bytes[1] = mouse_state.bytes[1];
  packet->bytes[2] = mouse_state.bytes[2];

  packet->lb = (packet->bytes[0] & BIT(0)) != 0; // left button
  packet->rb = (packet->bytes[0] & BIT(1)) != 0; // right button
  packet->mb = (packet->bytes[0] & BIT(2)) != 0; // middle button

  packet->delta_x = (int8_t)packet->bytes[1]; // x-displacement
  packet->delta_y = (int8_t)packet->bytes[2]; // y-displacement

  packet->x_ov = (packet->bytes[0] & BIT(6)) != 0; // x overflow
  packet->y_ov = (packet->bytes[0] & BIT(7)) != 0; // y overflow
}

void (mouse_ih)(void) {
  uint32_t status;
  if (sys_inb(KBC_STATUS_REG, &status) != OK) //skip if data read failed

  if (status & KBC_OUTPUT_BUFFER_FULL) { // output buffer full
    uint32_t data = 0;
    if (sys_inb(KBC_DATA_REG, &data) != OK) 

    if (!(status & (KBC_PARITY_ERROR | KBC_TIMEOUT_ERROR))) { // no errors
      if (mouse_state.byte_count < 3) {
        mouse_state.bytes[mouse_state.byte_count++] = (uint8_t)data;

        if (mouse_state.byte_count == 3) {
          parse_packet();
          mouse_state.packet_ready = true;
          mouse_state.packets_complete++;
          mouse_state.byte_count = 0;
        }
      }
    }
    // else -> byte discarded automatically
  }
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
