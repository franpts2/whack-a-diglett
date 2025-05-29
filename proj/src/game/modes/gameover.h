#ifndef GAMEOVER_H
#define GAMEOVER_H

#include <stdbool.h>
#include <stdint.h>

// Initialize game over screen
void gameover_init(void);

// Handle keyboard input
void gameover_handle_input(uint8_t scancode);

// Handle mouse actions
void gameover_handle_mouse(int x, int y, bool left_button_pressed);

// Draw game over screen
void gameover_draw(void);


#endif // GAMEOVER_H
