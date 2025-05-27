#include "cursor.h"
#include "../../controllers/video/video.h"
#include "../sprites/sprite.h"
#include "cursor_xpm.h"
#include <stdlib.h>

Cursor *cursor_init() {
  // Center of the screen (800x600 mode)
  int center_x = 400;
  int center_y = 300;

  return sprite_create_from_xpm((xpm_map_t) cursor_xpm, center_x, center_y);
}

int cursor_draw(Cursor *cursor) {
  return sprite_draw(cursor);
}

void cursor_set_position(Cursor *cursor, int x, int y) {
  if (cursor != NULL) {
    // Update cursor position
    cursor->x = x;
    cursor->y = y;
  }
}

void cursor_destroy(Cursor *cursor) {
  // Use the sprite destruction function
  sprite_destroy(cursor);
}
