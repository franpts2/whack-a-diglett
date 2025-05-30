/**
 * @file i8042.h
 * @brief Constants for the i8042 keyboard controller
 *
 * This file contains definitions of ports, bit masks, and commands
 * used to interact with the i8042 keyboard controller.
 */

#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

/** @brief Keyboard IRQ line */
#define IRQ_KEYBOARD    1

/** @brief ESC key breakcode */
#define BREAK_ESC       0x81

/** @brief Make/break code bit */
#define MAKE_CODE       BIT(7)

/** @brief Prefix for extended scancodes */
#define TWO_BYTES       0xE0

/** @brief Bit to enable interrupts in command byte */
#define ENABLE_INT      BIT(0)

/** @brief Bit indicating timeout error */
#define TIMEOUT_ERROR   BIT(6)

/** @brief Bit indicating parity error */
#define PARITY_ERROR    BIT(7)

/** @brief Bit indicating output buffer is full */
#define FULL_OUT_BUFFER BIT(0)

/** @brief Bit indicating input buffer is full */
#define FULL_IN_BUFFER  BIT(1)

/** @brief KBC status register port */
#define KBC_STATUS_REG  0x64

/** @brief KBC command input port */
#define KBC_IN_CMD      0x64

/** @brief KBC data port (input/output) */
#define KBC_OUT_CMD     0x60

/** @brief KBC command to read configuration byte */
#define KBC_READ_CMD    0x20

/** @brief KBC command to write configuration byte */
#define KBC_WRITE_CMD   0x60

/** @brief Wait time in microseconds for KBC operations */
#define WAIT_KBC        20000

/** @brief Maximum number of attempts for KBC operations */
#define MAX_ATTEMPS     10

#endif /* _LCOM_I8042_H_ */
