/**
 * @file keyboard.c
 * @brief Implementation of keyboard controller functions
 *
 * This file implements functions to handle keyboard interrupts and input.
 */

#include <lcom/lcf.h>
#include <stdint.h>
#include "keyboard.h"

/** @brief Keyboard IRQ hook ID */
static int keyboard_hook_id = 1;

/** @brief Last keyboard scancode read */
uint8_t scancode = 0;

/**
 * @brief Subscribes keyboard interrupts
 * 
 * @param bit_no Address of memory to be initialized with the bit number to be set in hook_id
 * @return Return 0 upon success and non-zero otherwise
 */
int (keyboard_subscribe_int)(uint8_t *bit_no) {
  
  if (bit_no == NULL) return 1;
  *bit_no = BIT(keyboard_hook_id);
  if (sys_irqsetpolicy(IRQ_KEYBOARD, IRQ_REENABLE | IRQ_EXCLUSIVE, &keyboard_hook_id) != 0) return 1; //interrupts subscription
  return 0;
}

/**
 * @brief Unsubscribes keyboard interrupts
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (keyboard_unsubscribe_int)() {
  if (sys_irqrmpolicy(&keyboard_hook_id) != 0) return 1;
  return 0;
}

/**
 * @brief Keyboard interrupt handler
 *
 * Reads the scancode from the keyboard controller and stores it in the global variable.
 * Prints an error message if reading fails.
 */
void (kbc_ih)() {
  if (read_KBC_output(KBC_OUT_CMD, &scancode, 0) != 0) printf("Error: Could not read scancode!\n");
}

/**
 * @brief Restores keyboard to default settings
 *
 * Reads the command byte, enables interrupts, and writes it back
 * to the keyboard controller.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (keyboard_restore)() {
  uint8_t commandByte;
  if (write_KBC_command(KBC_IN_CMD, KBC_READ_CMD) != 0) return 1;          
  if (read_KBC_output(KBC_OUT_CMD, &commandByte, 0) != 0) return 1; 
  commandByte |= ENABLE_INT;  
  if (write_KBC_command(KBC_IN_CMD, KBC_WRITE_CMD) != 0) return 1;    
  if (write_KBC_command(KBC_WRITE_CMD, commandByte) != 0) return 1;
  return 0;
}
