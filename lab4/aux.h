#include <lcom/lcf.h>

#include <lcom/lcf.h>
#include <minix/sysutil.h>


#define MOUSE_IRQ 12

#define KBC_STATUS_REG 0x64
#define KBC_CMD_REG 0x64
#define KBC_DATA_REG 0x60

#define KBC_INPUT_BUFFER_FULL BIT(1)
#define KBC_OUTPUT_BUFFER_FULL BIT(0)
#define KBC_PARITY_ERROR BIT(7)
#define KBC_TIMEOUT_ERROR BIT(6)

#define MAX_ATTEMPTS 10
#define WAIT_TIME 20000 // 20ms

#define ENABLE_DATA_REPORTING 0xF4
#define DISABLE_DATA_REPORTING 0xF5

#define ACK 0xFA
#define NACK 0xFE

#define WRITE_BYTE_MOUSE 0xD4
#define MOUSE_READ_DATA 0xEB
#define ENABLE_STREAM_MODE 0xEA

#define KBC_STATUS_REG 0x64
#define KBC_IN_COMMAND 0x64
#define KBC_OUT_COMMAND 0x60

#define OUT_BUF_FULL BIT(0)
#define IN_BUF_FULL BIT(1)

#define TIMEOUT_ERROR BIT(6)
#define PARITY_ERROR BIT(7)

#define FIRST_BYTE BIT(3)

#define MOUSE_LB BIT(0)
#define MOUSE_RB BIT(1)
#define MOUSE_MB BIT(2)

#define MOUSE_X_SIGNAL BIT(4)
#define MOUSE_Y_SIGNAL BIT(5)
#define MOUSE_X_OVERFLOW BIT(6)
#define MOUSE_Y_OVERFLOW BIT(7)


extern int hook_id_mouse;
extern uint8_t current_byte;

extern struct packet mouse_packet;
extern uint8_t byte_index;
extern uint8_t mouse_bytes[3];


int(mouse_subscribe_int)(uint8_t *bit_no);
int(mouse_unsubscribe_int)();

void(mouse_ih)();
void(mouse_collect_packet_byte)();
void(assemble_mouse_packet)();

int(write_to_mouse)(uint8_t command);
int(read_from_mouse)(uint8_t *output);

int(mouse_init)(uint8_t *mouse_mask);
int mouse_cleanup();

int(my_mouse_enable_data_reporting)();
int(mouse_disable_data_reporting)();

// KBC functions
int(read_KBC_status)(uint8_t *status);
int(read_KBC_output)(uint8_t port, uint8_t *output, uint8_t mouse);
int(write_KBC_command)(uint8_t port, uint8_t commandByte);

// Utility functions
int(util_sys_inb)(int port, uint8_t *value);
int(util_get_LSB)(uint16_t val, uint8_t *lsb);
int(util_get_MSB)(uint16_t val, uint8_t *msb);
