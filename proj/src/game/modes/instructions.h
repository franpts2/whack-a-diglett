#ifndef _INSTRUCTIONS_H
#define _INSTRUCTIONS_H

#include "../game.h"
#include <stdbool.h>
#include <stdint.h>

void instructions_init(void);
void instructions_handle_input(uint8_t scancode);
void draw_instructions(void);
void instructions_destroy(void);

#endif // _INSTRUCTIONS_H


