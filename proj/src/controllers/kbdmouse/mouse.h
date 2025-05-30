/**
 * @file mouse.h
 * @brief Header file for mouse controller
 *
 * This file contains functions related to mouse testing and gestures.
 */

#ifndef _MOUSE_H_
#define _MOUSE_H_

#include "i8042.h"
#include "kbc.h"
#include <lcom/lcf.h>
#include <stdint.h>
#include "aux.h"

/**
 * @brief Tests mouse packet reception
 * 
 * @param cnt Number of packets to receive and display
 * @return Return 0 upon success and non-zero otherwise
 */
int(mouse_test_packet)(uint32_t cnt);

/**
 * @brief Tests mouse interrupt handling using interrupts
 * 
 * @param idle_time Time in seconds after which the program should exit
 * @return Return 0 upon success and non-zero otherwise
 */
int(mouse_test_async)(uint8_t idle_time);

/**
 * @brief Tests detection of a gesture
 * 
 * @param x_len Minimum horizontal displacement
 * @param tolerance Maximum accepted deviation in y-coordinates
 * @return Return 0 upon success and non-zero otherwise
 */
int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance);

#endif // _MOUSE_H_