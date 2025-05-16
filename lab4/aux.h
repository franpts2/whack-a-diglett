#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#define MOUSE_IRQ 12

#define KBC_STATUS_REG  0x64
#define KBC_CMD_REG     0x64
#define KBC_DATA_REG    0x60

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


int(mouse_init)(uint8_t *mouse_mask);
void mouse_cleanup();

int (my_mouse_enable_data_reporting)();
int (mouse_disable_data_reporting)();

