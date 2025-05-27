#include "playing.h"
#include "../../controllers/kbdmouse/keyboard.h"
#include "../../controllers/video/video.h"
#include "../../fonts/testfont.h"
#include "../../game/game.h"
#include <stdio.h>

// initialize the playing screen
void playing_init(void) {
  // pinta o background
  vg_draw_rectangle(0, 0, 800, 600, 0x04);

  // titulo ?
  // draw_text_scaled("WHACK'A DIGGLET", 200, 50, 0xFF, 3);

  // um elemento de exemplo
  vg_draw_rectangle(450, 300, 100, 100, 0xFF);
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
