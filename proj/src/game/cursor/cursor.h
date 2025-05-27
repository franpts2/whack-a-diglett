#ifndef _CURSOR_H_
#define _CURSOR_H_

#include "../sprites/sprite.h"
#include "cursor_xpm.h"
#include <lcom/lcf.h>

struct packet;

typedef Sprite Cursor;

// Movement smoothing variables
extern int mouse_x_remainder;
extern int mouse_y_remainder;
extern float mouse_accel;

Cursor *cursor_init();

int cursor_draw(Cursor *cursor);

void cursor_set_position(Cursor *cursor, int x, int y);

void cursor_handle_mouse_packet(Cursor *cursor, struct packet *mouse_packet);

void cursor_destroy(Cursor *cursor);

#endif /* _CURSOR_H_ */
