#ifndef _PLAYING_MOUSE_H
#define _PLAYING_MOUSE_H

#include "playing.h"

// mouse-specific functions
void playing_mouse_init(void);
void playing_mouse_update(void);
void playing_handle_mouse_input(int x, int y, bool button_pressed);

#endif // _PLAYING_MOUSE_H
