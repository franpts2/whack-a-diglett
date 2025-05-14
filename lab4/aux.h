#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

static struct {
  uint8_t bytes[3];          // Current packet bytes
  unsigned byte_count;       // Bytes received for current packet
  volatile unsigned packets_complete; // Completed packets count
  bool packet_ready;         // Flag for new complete packet
  struct packet current;     // Parsed packet data
} mouse_state;


void (mouse_ih)(void);

static int mouse_init();

static void mouse_cleanup();
