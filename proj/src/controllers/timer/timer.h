/**
 * @file timer.h
 * @brief Header file for timer controller
 *
 * This file provides access to the timer controller functionality 
 * and the timer counter variable.
 */

#ifndef TIMER_H
#define TIMER_H

#include <lcom/lcf.h>
#include <stdint.h>

/** @brief Counter variable incremented by timer interrupts */
extern int counter;

/**
 * @brief Subscribes timer interrupts
 * 
 * @param bit_no Address of memory to be initialized with the bit number to be set in hook_id
 * @return Return 0 upon success and non-zero otherwise
 */
int (timer_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes timer interrupts
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (timer_unsubscribe_int)();

/**
 * @brief Timer interrupt handler
 *
 * Increments the counter variable on each timer interrupt.
 */
void (timer_int_handler)();

/**
 * @brief Sets the frequency of a timer
 *
 * @param timer Timer to configure (0, 1, or 2)
 * @param freq Frequency to set in Hz
 * @return Return 0 upon success and non-zero otherwise
 */
int (timer_set_frequency)(uint8_t timer, uint32_t freq);

/**
 * @brief Gets configuration of the specified timer
 *
 * @param timer Timer to read configuration from (0, 1, or 2)
 * @param st Address to store the read configuration
 * @return Return 0 upon success and non-zero otherwise
 */
int (timer_get_conf)(uint8_t timer, uint8_t *st);

/**
 * @brief Displays timer configuration
 *
 * @param timer Timer whose configuration to display (0, 1, or 2)
 * @param st Configuration status byte to parse
 * @param field Specific field of the configuration to display
 * @return Return 0 upon success and non-zero otherwise
 */
int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field);

#endif // TIMER_H
