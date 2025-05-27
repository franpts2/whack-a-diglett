#ifndef PLAYING_H
#define PLAYING_H

#include <stdint.h>

void playing_init(void);

void playing_handle_input(uint8_t scancode);

void playing_update(void);

void update_diglett_visibility(int index);

void draw_points_counter(void);

#endif // PLAYING_H
