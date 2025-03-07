#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int counter = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
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
      val.count_mode = (st>>1) & (BIT(1) | BIT(2) | BIT(3));

      if (st==6) val.count_mode = 2;
      else if (st==7) val.count_mode = 3;
      else val.count_mode = st;

      break;

    case tsf_initial:
      val.in_mode = (st >> 4) & (BIT(4) | BIT(5));

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
