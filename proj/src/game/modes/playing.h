#ifndef PLAYING_H
#define PLAYING_H

#include <stdint.h>

// Initialize the playing mode
void playing_init(void);

// Handle input in playing mode
void playing_handle_input(uint8_t scancode);

// Update the playing mode
void playing_update(void);

#endif // PLAYING_H
