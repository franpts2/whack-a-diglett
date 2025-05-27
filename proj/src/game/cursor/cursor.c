#include "cursor.h"
#include "../../controllers/kbdmouse/aux.h"
#include "../../controllers/video/video.h"
#include "../sprites/sprite.h"
#include "cursor_xpm.h"
#include <stdlib.h>

// Initialize movement smoothing variables
int mouse_x_remainder = 0;
int mouse_y_remainder = 0;
float mouse_accel = 1.0;

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
    cursor->x = x;
    cursor->y = y;
  }
}

void cursor_destroy(Cursor *cursor) {
  sprite_destroy(cursor);
}

void cursor_handle_mouse_packet(Cursor *cursor, struct packet *mouse_packet) {
  if (cursor == NULL || mouse_packet == NULL) {
    return;
  }

  float sensitivity = 2;

  // dynamic acceleration based on movement speed
  int dx_abs = abs(mouse_packet->delta_x);
  int dy_abs = abs(mouse_packet->delta_y);

  int movement_intensity = (dx_abs > dy_abs) ? dx_abs + (dy_abs / 2) : dy_abs + (dx_abs / 2);

  // more natural, smooth acceleration based on movement intensity
  if (movement_intensity > 12) {
    mouse_accel = 2.0; // hyper fast
  }
  else if (movement_intensity > 8) {
    mouse_accel = 1.7; // fast
  }
  else if (movement_intensity > 4) {
    mouse_accel = 1.4; // medium speed
  }
  else if (movement_intensity > 2) {
    mouse_accel = 1.2; // slow
  }
  else {
    mouse_accel = 1.0; // very slow movement
  }

  // 'fractional' movement tracking for precise movemnts using higher precision (4 extra bits)
  mouse_x_remainder += mouse_packet->delta_x * 16;
  mouse_y_remainder += mouse_packet->delta_y * 16;

  int dx_applied = (mouse_x_remainder * sensitivity * mouse_accel) / 16;
  int dy_applied = (mouse_y_remainder * sensitivity * mouse_accel) / 16;

  // remove applied movement from remainder
  mouse_x_remainder -= (dx_applied * 10) / (sensitivity * mouse_accel);
  mouse_y_remainder -= (dy_applied * 10) / (sensitivity * mouse_accel);

  int new_x = cursor->x + dx_applied;
  int new_y = cursor->y - dy_applied; // y is inverted

  // keep inside screen (800x600)
  if (new_x < 0)
    new_x = 0;
  if (new_x > 800 - cursor->width)
    new_x = 800 - cursor->width;
  if (new_y < 0)
    new_y = 0;
  if (new_y > 600 - cursor->height)
    new_y = 600 - cursor->height;

  cursor_set_position(cursor, new_x, new_y);
}
