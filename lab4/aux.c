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

int(mouse_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL)
    return 1;
  *bit_no = BIT(hook_id_mouse);
  return sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_mouse);
}

int(mouse_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id_mouse);
}

void(mouse_ih)() {
  if (read_KBC_output(KBC_OUT_COMMAND, &current_byte, 1))
    printf("couldn't read byte from mouse\n");
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

int(mouse_init)(uint8_t *mouse_mask) {
  if (mouse_subscribe_int(mouse_mask) != 0)
    return 1;

  if (my_mouse_enable_data_reporting() != 0)
    return 1;

  return 0;
}

int(mouse_cleanup)() {
  if (mouse_disable_data_reporting() != 0)
    return 1;

  if (mouse_unsubscribe_int() != 0)
    return 1;

  return 0;
}

int(my_mouse_enable_data_reporting)() {
  if (write_to_mouse(ENABLE_DATA_REPORTING) != 0)
    return 1;

  return 0;
}

int(mouse_disable_data_reporting)() {
  if (write_to_mouse(DISABLE_DATA_REPORTING) != 0)
    return 1;

  return 0;
}

// ===== KBC Functions =====

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

// ===== Utility Functions =====

int(util_sys_inb)(int port, uint8_t *value) {
  if (value == NULL)
    return 1;

  uint32_t val;
  if (sys_inb(port, &val) != 0)
    return 1;

  *value = (uint8_t) (val & 0xFF);
  return 0;
}

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (lsb == NULL)
    return 1;

  *lsb = (uint8_t) (val & 0xFF);
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if (msb == NULL)
    return 1;

  *msb = (uint8_t) (val >> 8);
  return 0;
}

// ===== Gesture Detection auxiliary functions =====

gesture_state_t(process_init_state)(struct packet *packet) {
  // start of the first line: LB pressed, no other buttons
  if (packet->lb && !packet->rb && !packet->mb) {
    return DRAW_UP; // transition to DRAW_UP state
  }
  return INIT;
}

gesture_state_t(process_draw_up_state)(struct packet *packet, int16_t *x_disp, int16_t *y_disp, uint8_t x_len, uint8_t tolerance) {
  // check if LB is still pressed
  if (packet->lb && !packet->rb && !packet->mb) {
    // check if movement is mostly upward and to the right
    // allow for small negative movements within tolerance
    if (packet->delta_x < 0 && ((-1) * packet->delta_x > tolerance)) {
      return INIT; // negative X movement beyond tolerance - reset state
    } 
    else if (packet->delta_y < 0 && ((-1) * packet->delta_y > tolerance)) {
      return INIT; // negative Y movement beyond tolerance - reset state
    }
    else {
      // valid movement, update displacements
      *x_disp += packet->delta_x;
      *y_disp += packet->delta_y;
      return DRAW_UP; // stay in DRAW_UP state
    }
  }
  // check if LB was released - potential transition to VERTEX state
  else if (!packet->lb && !packet->rb && !packet->mb) {
    if (is_line_valid(*x_disp, *y_disp, x_len)) {
      return VERTEX; // transition to VERTEX state
    } else {
      return INIT; // doesn't meet requirements - reset state
    }
  } 
  else {
    // other buttons were pressed - reset state
    return INIT;
  }
}

gesture_state_t(process_vertex_state)(struct packet *packet, uint8_t tolerance) {
  // at the vertex, we should have no buttons pressed and small movements only (within tolerance)
  if (packet->rb && !packet->lb && !packet->mb) {
    // right button pressed - move to next line
    return DRAW_DOWN; // transition to DRAW_DOWN state
  }
  else if (!packet->lb && !packet->rb && !packet->mb) {
    // no buttons pressed - check if movement is within tolerance
    if (!is_movement_within_tolerance(packet, tolerance)) {
      return INIT; // movement too large - reset state
    }
    return VERTEX;
  }
  else {
    return INIT; // invalid button combination - reset state
  }
}

gesture_state_t(process_draw_down_state)(struct packet *packet, int16_t *x_disp, int16_t *y_disp, uint8_t x_len, uint8_t tolerance) {
  // check if RB is still pressed
  if (packet->rb && !packet->lb && !packet->mb) {
    // check if movement is mostly downward and to the right
    if (packet->delta_x < 0 && ((-1) * packet->delta_x > tolerance)) {
      return INIT;
    }
    else if (packet->delta_y < 0 && ((-1) * packet->delta_y > tolerance)) {
      return INIT;
    }
    else {
      // valid movement, update displacements
      *x_disp += packet->delta_x;
      *y_disp += abs(packet->delta_y); 
      return DRAW_DOWN;
    }
  }
  // check if RB was released - potential completion of gesture
  else if (!packet->lb && !packet->rb && !packet->mb) {
    // check if the second line meets the requirements
    if (is_line_valid(*x_disp, *y_disp, x_len)) {
      return COMPLETE; // gesture completed
    } else {
      return INIT; // line doesn't meet requirements - reset state
    }
  }
  else {
    return INIT; // other buttons were pressed - reset state
  }
}

bool(is_line_valid)(int16_t x_displacement, int16_t y_displacement, uint8_t x_len) {
  // check if line meets the requirements:
  // 1. x displacement >= x_len
  // 2. |slope| > 1 (y displacement > x displacement)
  return (x_displacement >= x_len && y_displacement > x_displacement);
}

bool(is_movement_within_tolerance)(struct packet *packet, uint8_t tolerance) {
  // check if movement is within tolerance in both axes
  return (abs(packet->delta_x) <= tolerance && abs(packet->delta_y) <= tolerance);
}
