/**
 * @file kbc.h
 * @brief Header file for keyboard controller (KBC) functions
 *
 * This file provides low-level functions to interact with the
 * keyboard controller for both keyboard and mouse devices.
 */

#ifndef _LCOM_KBC_H_
#define _LCOM_KBC_H_

#include <minix/sysutil.h>
#include "i8042.h"
#include <lcom/lcf.h>

/**
 * @brief Reads the keyboard controller status register
 *
 * @param status Pointer to variable where status byte will be stored
 * @return 0 on success, non-zero otherwise
 */
int (read_KBC_status)(uint8_t* status);

/**
 * @brief Reads output from the keyboard controller
 *
 * @param port Port to read from (usually 0x60)
 * @param output Pointer to variable where output byte will be stored
 * @param mouse Flag indicating if reading from mouse (1) or keyboard (0)
 * @return 0 on success, non-zero otherwise
 */
int (read_KBC_output)(uint8_t port, uint8_t *output, uint8_t mouse);

/**
 * @brief Writes a command to the keyboard controller
 *
 * @param port Port to write to (usually 0x64 for commands)
 * @param commandByte Command byte to write
 * @return 0 on success, non-zero otherwise
 */
int (write_KBC_command)(uint8_t port, uint8_t commandByte);

#endif /* _LCOM_KBC_H_ */
