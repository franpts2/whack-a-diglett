#include "cursor.h"
#include "../../controllers/video/video.h"
#include "../sprites/sprite.h"
#include "cursor_xpm.h"
#include <stdlib.h>

Cursor *cursor_init() {
  Cursor *cursor = (Cursor *) malloc(sizeof(Cursor));
  if (cursor == NULL) {
    printf("Failed to allocate memory for cursor\n");
    return NULL;
  }

  // Initialize cursor at the center of the screen (assuming 1024x768 mode)
  cursor->x = 512;
  cursor->y = 384;
  cursor->is_visible = true;

  // Load the cursor XPM image
  cursor->pixmap = (uint32_t *) xpm_load((xpm_map_t) cursor_xpm, XPM_8_8_8_8, &cursor->img);
  if (cursor->pixmap == NULL) {
    free(cursor);
    return NULL;
  }

  return cursor;
}

int cursor_draw(Cursor *cursor) {
  // Use the generic sprite_draw function since Cursor is now a Sprite
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
  if (cursor != NULL) {
    if (cursor->pixmap != NULL) {
      free(cursor->pixmap);
    }
    free(cursor);
  }
}
