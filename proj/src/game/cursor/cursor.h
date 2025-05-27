#ifndef _CURSOR_H_
#define _CURSOR_H_

#include "cursor_xpm.h"
#include <lcom/lcf.h>

typedef struct {
  int x;
  int y;
  uint32_t *pixmap;
  xpm_image_t img;
  bool is_visible; 
} Cursor;

Cursor *cursor_init();

int cursor_draw(Cursor *cursor);

void cursor_set_position(Cursor *cursor, int x, int y);

void cursor_destroy(Cursor *cursor);

#endif /* _CURSOR_H_ */
