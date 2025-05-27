#ifndef _PLAYING_KBD_H
#define _PLAYING_KBD_H

#include "playing.h"

// keyboard-specific functions
void playing_kbd_init(void);
void playing_kbd_update(void);
void playing_handle_input(uint8_t scancode);
void draw_kbd_diglett(int index);

// external declarations
extern const uint8_t diglett_keys[NUM_DIGLETTS];

#endif // _PLAYING_KBD_H
