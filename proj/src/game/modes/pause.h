#ifndef _PAUSE_H_
#define _PAUSE_H_

#include "../game.h"
#include <stdbool.h>
#include <stdint.h>

// Function declarations for pause state
void pause_init(void);
void pause_handle_input(uint8_t scancode);
void draw_pause_screen(void);
void pause_resume_game(void);

#endif // _PAUSE_H_
