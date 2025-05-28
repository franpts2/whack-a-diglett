#include "playing_mouse.h"
#include "../../../controllers/video/video.h"
#include "../../../fonts/testfont.h"
#include "playing.h"
#include <stdbool.h>
#include <stdio.h>

void draw_mouse_diglett(int index) {
  draw_diglett(index, false);
}

void playing_mouse_update(void) {
  playing_update(false);
}

void playing_mouse_init(void) {
  extern Cursor *g_cursor;
  extern int mode_selected;

  if (g_cursor != NULL && mode_selected == 1) {
    cursor_set_position(g_cursor, 400, 300);
  }

  // false = mouse mode
  playing_init(false);
}

void playing_handle_mouse_input(int x, int y, bool button_pressed) {
  if (!button_pressed) {
    return;
  }

  // Check if the click coordinates are inside any diglett's area
  for (int i = 0; i < NUM_DIGLETTS; i++) {
    if (!digletts[i].active) {
      continue;
    }

    if (x >= digletts[i].x &&
        x <= digletts[i].x + digletts[i].width &&
        y >= digletts[i].y &&
        y <= digletts[i].y + digletts[i].height) {

      playing_handle_common_input(i, false);
      return;
    }
  }
}
