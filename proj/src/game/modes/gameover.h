#ifndef GAMEOVER_H
#define GAMEOVER_H

#include <stdbool.h>
#include <stdint.h>

// Initialize game over screen
void gameover_init(void);

// Handle keyboard input
void gameover_handle_input(uint8_t scancode);

// Draw game over screen
void gameover_draw(void);


#endif // GAMEOVER_H
