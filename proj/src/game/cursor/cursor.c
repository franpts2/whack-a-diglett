#include "cursor.h"
#include "../../controllers/video/video.h"
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
  if (cursor == NULL || cursor->pixmap == NULL || !cursor->is_visible)
    return 1;

  // Directly draw the cursor pixmap
  for (uint16_t row = 0; row < cursor->img.height; row++) {
    for (uint16_t col = 0; col < cursor->img.width; col++) {
      uint32_t color_index = cursor->pixmap[row * cursor->img.width + col];

      if (color_index == xpm_transparency_color(XPM_8_8_8_8)) {
        // Skip drawing transparent pixels (assuming 0xFFFFFFFF is the transparent color)
        continue;
      }
      // Skip drawing transparent pixels
      if (color_index != 0) {
        vg_draw_rectangle(cursor->x + col, cursor->y + row, 1, 1, color_index);
      }
    }
  }

  return 0;
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
