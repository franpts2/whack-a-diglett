/**
 * @file keyboard.h
 * @brief Header file for keyboard controller
 *
 * This file contains functions to handle keyboard interrupts and keyboard input.
 */

#ifndef _LCOM_KEYBOARD_H_
#define _LCOM_KEYBOARD_H_

#include "kbc.h"
#include "i8042.h"

/** @brief Last keyboard scancode read */
extern uint8_t scancode;

/**
 * @brief Subscribes keyboard interrupts
 * 
 * @param bit_no Address of memory to be initialized with the bit number to be set in hook_id
 * @return Return 0 upon success and non-zero otherwise
 */
int (keyboard_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes keyboard interrupts
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (keyboard_unsubscribe_int)();

/**
 * @brief Keyboard interrupt handler
 *
 * Reads the scancode from the keyboard controller and stores it in the global variable.
 */
void (kbc_ih)();

/**
 * @brief Restores keyboard to default settings
 *
 * Restores keyboard controller to default settings and enables interrupts.
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int keyboard_restore();

#endif
