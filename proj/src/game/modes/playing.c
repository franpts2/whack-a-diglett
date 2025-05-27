#include "playing.h"
#include "../../controllers/kbdmouse/keyboard.h"
#include "../../controllers/video/video.h"
#include "../../fonts/testfont.h"
#include "../../game/game.h"
#include <stdio.h>

// initialize the playing screen
void playing_init(void) {
  // pinta o background
  vg_draw_rectangle(0, 0, 800, 600, 0x325918);

  // titulo
  draw_text_scaled("WHACK'A DIGGLET", 290, 50, 0xFFFFFF, 3);

  int rect_width = 60;
  int rect_height = 80;
  int spacing = 60; // space between digletts

  int grid_width = 3 * rect_width + 2 * spacing;
  // int grid_height = 3 * rect_height + 2 * spacing;

  // calculate starting position to center the grid
  int start_x = (800 - grid_width) / 2;
  int start_y = 150;

  // 3x3 grid
  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
      int x = start_x + col * (rect_width + spacing);
      int y = start_y + row * (rect_height + spacing);

      vg_draw_rectangle(x, y, rect_width, rect_height, 0x885500); // cor ainda por definir
    }
  }
}

// handles keyboard input in playing mode
void playing_handle_input(uint8_t scancode) {
  extern GameMode current_mode;

  // q key to returns to menu
  if (scancode == 0x10) {
    current_mode = MODE_MENU;
  }

  // adicionar controles
}

// update game state
void playing_update(void) {
  // update game objects animations etc
  // will be called each frame in the game loop
}
