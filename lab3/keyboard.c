#include <lcom/lcf.h>
#include <stdint.h>
#include "keyboard.h"

static int keyboard_hook_id = 1;
uint8_t scancode = 0;


int (keyboard_subscribe_int)(uint8_t *bit_no) {
  
  if (bit_no == NULL) return 1;

  *bit_no = BIT(keyboard_hook_id);

  if (sys_irqsetpolicy(IRQ_KEYBOARD, IRQ_REENABLE | IRQ_EXCLUSIVE, &keyboard_hook_id) != OK) return 1; //interrupts subscription

  return 0;
}

int (keyboard_unsubscribe_int)() {
  if (sys_irqrmpolicy(&keyboard_hook_id) != OK) return 1;
  return 0;
}

void (kbc_ih)() {
  if (read_KBC_output(KBC_OUT_CMD, &scancode, 0) != 0) printf("Error: Could not read scancode!\n");
}

int (keyboard_restore)() {
  uint8_t commandByte;

  if (write_KBC_command(KBC_IN_CMD, KBC_READ_CMD) != 0) return 1;          
  if (read_KBC_output(KBC_OUT_CMD, &commandByte, 0) != 0) return 1; 

  commandByte |= ENABLE_INT;  

  if (write_KBC_command(KBC_IN_CMD, KBC_WRITE_CMD) != 0) return 1;    
  if (write_KBC_command(KBC_WRITE_CMD, commandByte) != 0) return 1;

  return 0;
}
