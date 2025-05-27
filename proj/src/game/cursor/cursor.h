#ifndef _CURSOR_H_
#define _CURSOR_H_

#include "../sprites/sprite.h"
#include "cursor_xpm.h"
#include <lcom/lcf.h>

typedef Sprite Cursor;

Cursor *cursor_init();

int cursor_draw(Cursor *cursor);

void cursor_set_position(Cursor *cursor, int x, int y);

void cursor_destroy(Cursor *cursor);

#endif /* _CURSOR_H_ */
