#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int hook_id = 0;
int counter = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  if (freq < 19 || freq > TIMER_FREQ) return 1;

  uint8_t st;
  if (timer_get_conf(timer, &st)) return 1;

  uint8_t ctrl = (st & 0x0F) | TIMER_LSB_MSB;
  uint8_t port;
  switch (timer) {
    case 0: ctrl |= TIMER_SEL0; port = TIMER_0; break;
    case 1: ctrl |= TIMER_SEL1; port = TIMER_1; break;
    case 2: ctrl |= TIMER_SEL2; port = TIMER_2; break;
    default: return 1;
  }

  uint32_t div = TIMER_FREQ / freq;
  uint8_t lsb, msb;
  util_get_LSB(div, &lsb);
  util_get_MSB(div, &msb);

  if (sys_outb(TIMER_CTRL, ctrl)) return 1;
  if (sys_outb(port, lsb)) return 1;
  if (sys_outb(port, msb)) return 1;
  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  
  if (bit_no == NULL) return 1;

  *bit_no = BIT(hook_id);

  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != OK) return 1;

  return 0;
}

int (timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id) != OK) return 1;
  return 0;
}

void (timer_int_handler)() {
  counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  if (st == NULL) return 1; 
  uint32_t cmd = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
  if (sys_outb(TIMER_CTRL, cmd) != OK) return 1;
  int read_port;
    switch (timer) {
        case 0:
            read_port = TIMER_0;
            break;
        case 1:
            read_port = TIMER_1;
            break;
        case 2:
            read_port = TIMER_2;
            break;
        default:
            return 1;
    }
  if (util_sys_inb(read_port, st) != 0) return 1;
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  union timer_status_field_val val;

  switch (field) {
    case tsf_all:
      val.byte = st;
      break;

    case tsf_base:
      val.bcd = st & BIT(0);
      break;

    case tsf_mode:
      st = (st>>1) & 0x07; //extracts bits 1-3

      if (st==6) val.count_mode = 2;
      else if (st==7) val.count_mode = 3;
      else val.count_mode = st;

      break;

    case tsf_initial:
      st = (st >> 4) & 0x03; //extracts bits 4-5

      if (st==1) val.in_mode = LSB_only;
      else if (st==2) val.in_mode = MSB_only;
      else if (st==3) val.in_mode = MSB_after_LSB;
      else val.in_mode = INVAL_val;

      break;

    default:
      return 1;
  }

  if (timer_print_config(timer, field, val)!=0) return 1;
  return 0;
}