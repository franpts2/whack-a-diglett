#ifndef _PLAYING_KBD_H
#define _PLAYING_KBD_H

#include "playing.h"


void playing_init(void);

void playing_handle_input(uint8_t scancode);

void playing_update(void);

// void update_diglett_visibility(int index);

void draw_diglett(int index);

void draw_points_counter(void);

#endif // _PLAYING_KBD_H
