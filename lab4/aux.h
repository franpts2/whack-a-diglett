#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#define MOUSE_IRQ 12

#define KBC_INPUT_BUFFER_FULL BIT(1)
#define KBC_OUTPUT_BUFFER_FULL BIT(0)
#define KBC_PARITY_ERROR BIT(7)
#define KBC_TIMEOUT_ERROR BIT(6)
#define MAX_ATTEMPTS 5
#define DELAY_US 20000 // 20ms

static struct {
  uint8_t bytes[3];                   // Current packet bytes
  unsigned byte_count;                // Bytes received for current packet
  volatile unsigned packets_complete; // Completed packets count
  bool packet_ready;                  // Flag for new complete packet
  struct packet current;              // Parsed packet data
} mouse_state;


int mouse_init();
void mouse_cleanup();
int (my_mouse_enable_data_reporting)();
int (mouse_disable_data_reporting)();

