/**
 * @file aux.h
 * @brief Auxiliary functions for mouse and keyboard controller
 *
 * This file provides constants, data structures, and functions for mouse 
 * packet processing and keyboard controller interaction.
 */

#include <lcom/lcf.h>
#include <minix/sysutil.h>

/** @brief Mouse IRQ line */
#define MOUSE_IRQ 12

/** @brief Keyboard controller status register port */
#define KBC_STATUS_REG 0x64

/** @brief Keyboard controller command register port */
#define KBC_CMD_REG 0x64

/** @brief Keyboard controller data register port */
#define KBC_DATA_REG 0x60

/** @brief Status bit for input buffer full */
#define KBC_INPUT_BUFFER_FULL BIT(1)

/** @brief Status bit for output buffer full */
#define KBC_OUTPUT_BUFFER_FULL BIT(0)

/** @brief Status bit for parity error */
#define KBC_PARITY_ERROR BIT(7)

/** @brief Status bit for timeout error */
#define KBC_TIMEOUT_ERROR BIT(6)

/** @brief Maximum number of attempts for controller communication */
#define MAX_ATTEMPTS 10

/** @brief Wait time in microseconds between attempts */
#define WAIT_TIME 20000 // 20ms

/** @brief Mouse command to enable data reporting */
#define ENABLE_DATA_REPORTING 0xF4

/** @brief Mouse command to disable data reporting */
#define DISABLE_DATA_REPORTING 0xF5

/** @brief Acknowledgment byte from the mouse */
#define ACK 0xFA

/** @brief Negative acknowledgment byte from the mouse */
#define NACK 0xFE

/** @brief Command to write byte to mouse */
#define WRITE_BYTE_MOUSE 0xD4

/** @brief Mouse command to read data */
#define MOUSE_READ_DATA 0xEB

/** @brief Mouse command to enable stream mode */
#define ENABLE_STREAM_MODE 0xEA

/** @brief Status register address */
#define KBC_STATUS_REG 0x64

/** @brief Input command port */
#define KBC_IN_COMMAND 0x64

/** @brief Output command port */
#define KBC_OUT_COMMAND 0x60

/** @brief Status bit for output buffer full */
#define OUT_BUF_FULL BIT(0)

/** @brief Status bit for input buffer full */
#define IN_BUF_FULL BIT(1)

/** @brief Status bit for timeout error */
#define TIMEOUT_ERROR BIT(6)

/** @brief Status bit for parity error */
#define PARITY_ERROR BIT(7)

/** @brief Flag indicating first byte of mouse packet */
#define FIRST_BYTE BIT(3)

/** @brief Bit for left mouse button */
#define MOUSE_LB BIT(0)

/** @brief Bit for right mouse button */
#define MOUSE_RB BIT(1)

/** @brief Bit for middle mouse button */
#define MOUSE_MB BIT(2)

/** @brief Bit for X movement sign */
#define MOUSE_X_SIGNAL BIT(4)

/** @brief Bit for Y movement sign */
#define MOUSE_Y_SIGNAL BIT(5)

/** @brief Bit for X overflow */
#define MOUSE_X_OVERFLOW BIT(6)

/** @brief Bit for Y overflow */
#define MOUSE_Y_OVERFLOW BIT(7)

/** @brief Hook ID for mouse interrupts */
extern int hook_id_mouse;

/** @brief Current byte read from mouse */
extern uint8_t current_byte;

/** @brief Mouse packet structure with parsed data */
extern struct packet mouse_packet;

/** @brief Index of current byte in packet assembly */
extern uint8_t byte_index;

/** @brief Array to store the 3 bytes of a mouse packet */
extern uint8_t mouse_bytes[3];

/**
 * @brief Gesture state machine states
 *
 * Enumeration of states for the gesture detection state machine.
 */
typedef enum {
  INIT,      /**< Initial state, waiting for left button press */
  DRAW_UP,   /**< Drawing the first line (left button pressed) */
  VERTEX,    /**< At the vertex, between the release of LB and press of RB */
  DRAW_DOWN, /**< Drawing the second line (right button pressed) */
  COMPLETE   /**< Gesture completed successfully */
} gesture_state_t;

/**
 * @brief Subscribes mouse interrupts
 * 
 * @param bit_no Address to store the bit number for the interrupt mask
 * @return 0 upon success and non-zero otherwise
 */
int(mouse_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes mouse interrupts
 * 
 * @return 0 upon success and non-zero otherwise
 */
int(mouse_unsubscribe_int)();

/**
 * @brief Mouse interrupt handler
 *
 * Reads a byte from the mouse data port.
 */
void(mouse_ih)();

/**
 * @brief Collects bytes to assemble a mouse packet
 *
 * Stores bytes in the mouse_bytes array and updates the byte_index.
 */
void(mouse_collect_packet_byte)();

/**
 * @brief Assembles a mouse packet from the collected bytes
 *
 * Parses the bytes in mouse_bytes to fill the mouse_packet structure.
 */
void(assemble_mouse_packet)();

/**
 * @brief Writes a command to the mouse
 * 
 * @param command Command byte to write
 * @return 0 upon success and non-zero otherwise
 */
int(write_to_mouse)(uint8_t command);

/**
 * @brief Reads output from the mouse
 * 
 * @param output Address to store the read byte
 * @return 0 upon success and non-zero otherwise
 */
int(read_from_mouse)(uint8_t *output);

/**
 * @brief Initializes the mouse
 * 
 * @param mouse_mask Address to store the mouse interrupt bit
 * @return 0 upon success and non-zero otherwise
 */
int(mouse_init)(uint8_t *mouse_mask);

/**
 * @brief Cleans up mouse resources
 * 
 * @return 0 upon success and non-zero otherwise
 */
int mouse_cleanup();

/**
 * @brief Enables mouse data reporting
 * 
 * @return 0 upon success and non-zero otherwise
 */
int(my_mouse_enable_data_reporting)();

/**
 * @brief Disables mouse data reporting
 * 
 * @return 0 upon success and non-zero otherwise
 */
int(mouse_disable_data_reporting)();

/**
 * @brief Reads keyboard controller status
 * 
 * @param status Address to store the status byte
 * @return 0 upon success and non-zero otherwise
 */
int(read_KBC_status)(uint8_t *status);

/**
 * @brief Reads output from keyboard controller
 * 
 * @param port Port to read from
 * @param output Address to store the output byte
 * @param mouse Flag indicating if reading from mouse
 * @return 0 upon success and non-zero otherwise
 */
int(read_KBC_output)(uint8_t port, uint8_t *output, uint8_t mouse);

/**
 * @brief Writes a command to the keyboard controller
 * 
 * @param port Port to write to
 * @param commandByte Command byte to write
 * @return 0 upon success and non-zero otherwise
 */
int(write_KBC_command)(uint8_t port, uint8_t commandByte);

/**
 * @brief Reads a byte from an I/O port
 * 
 * @param port The port to read from
 * @param value Address to store the read value
 * @return 0 upon success and non-zero otherwise
 */
int(util_sys_inb)(int port, uint8_t *value);

/**
 * @brief Gets the least significant byte of a word
 * 
 * @param val The input word
 * @param lsb Address to store the least significant byte
 * @return 0 upon success and non-zero otherwise
 */
int(util_get_LSB)(uint16_t val, uint8_t *lsb);

/**
 * @brief Gets the most significant byte of a word
 * 
 * @param val The input word
 * @param msb Address to store the most significant byte
 * @return 0 upon success and non-zero otherwise
 */
int(util_get_MSB)(uint16_t val, uint8_t *msb);

/**
 * @brief Processes INIT state of gesture detection
 * 
 * @param packet Pointer to the mouse packet
 * @return Next state in the gesture state machine
 */
gesture_state_t(process_init_state)(struct packet *packet);

/**
 * @brief Processes DRAW_UP state of gesture detection
 * 
 * @param packet Pointer to the mouse packet
 * @param x_disp Pointer to store X displacement
 * @param y_disp Pointer to store Y displacement
 * @param x_len Minimum required X displacement
 * @param tolerance Maximum allowed deviation in Y
 * @return Next state in the gesture state machine
 */
gesture_state_t(process_draw_up_state)(struct packet *packet, int16_t *x_disp, int16_t *y_disp, uint8_t x_len, uint8_t tolerance);

/**
 * @brief Processes VERTEX state of gesture detection
 * 
 * @param packet Pointer to the mouse packet
 * @param tolerance Maximum allowed movement in vertex
 * @return Next state in the gesture state machine
 */
gesture_state_t(process_vertex_state)(struct packet *packet, uint8_t tolerance);

/**
 * @brief Processes DRAW_DOWN state of gesture detection
 * 
 * @param packet Pointer to the mouse packet
 * @param x_disp Pointer to store X displacement
 * @param y_disp Pointer to store Y displacement
 * @param x_len Minimum required X displacement
 * @param tolerance Maximum allowed deviation in X
 * @return Next state in the gesture state machine
 */
gesture_state_t(process_draw_down_state)(struct packet *packet, int16_t *x_disp, int16_t *y_disp, uint8_t x_len, uint8_t tolerance);

/**
 * @brief Checks if line displacement meets requirements
 * 
 * @param x_displacement X displacement of the line
 * @param y_displacement Y displacement of the line
 * @param x_len Minimum required X displacement
 * @return True if the line is valid, false otherwise
 */
bool(is_line_valid)(int16_t x_displacement, int16_t y_displacement, uint8_t x_len);

/**
 * @brief Checks if mouse movement is within tolerance
 * 
 * @param packet Pointer to the mouse packet
 * @param tolerance Maximum allowed movement
 * @return True if within tolerance, false otherwise
 */
bool(is_movement_within_tolerance)(struct packet *packet, uint8_t tolerance);
