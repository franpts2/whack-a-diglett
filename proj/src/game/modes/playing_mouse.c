#include "playing_mouse.h"
#include "../../controllers/video/video.h"
#include "../../fonts/testfont.h"
#include "playing.h"
#include <stdbool.h>
#include <stdio.h>

void draw_mouse_diglett(int index) {
  // to do
  draw_diglett(index, false);
}


void playing_mouse_update(void) {
  // to do
  playing_update(false);
}

// Mouse-specific initialization
void playing_mouse_init(void) {
  // to do
  playing_init(false);
}

void playing_handle_mouse_input(int x, int y, bool button_pressed) {
  // to do
}
