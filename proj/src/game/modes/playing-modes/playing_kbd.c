#include "playing_kbd.h"
#include "../../../controllers/kbdmouse/keyboard.h"
#include "../../../controllers/video/video.h"
#include "../../../fonts/testfont.h"
#include "../pause.h"
#include "playing.h"
#include <stdbool.h>
#include <stdio.h>

// scancodes for the keys assigned to digletts
const uint8_t diglett_keys[NUM_DIGLETTS] = {
  0x13, 0x14, 0x15, // r, t, y (first row)
  0x21, 0x22, 0x23, // f, g, h (second row)
  0x2F, 0x30, 0x31  // v, b, n (third row)
};

void draw_kbd_diglett(int index) {
  draw_diglett(index, true); //keyboard mode enabled
}

void draw_kbd_diglett_label(int index) {
  Diglett *dig = &digletts[index];

  char key_label[2] = {0};
  switch (dig->key) {
    case 0x13: key_label[0] = 'r'; break;
    case 0x14: key_label[0] = 't'; break;
    case 0x15: key_label[0] = 'y'; break;
    case 0x21: key_label[0] = 'f'; break;
    case 0x22: key_label[0] = 'g'; break;
    case 0x23: key_label[0] = 'h'; break;
    case 0x2F: key_label[0] = 'v'; break;
    case 0x30: key_label[0] = 'b'; break;
    case 0x31: key_label[0] = 'n'; break;
  }
  draw_text_scaled(key_label, dig->x + dig->width - 14, dig->y + 5, 0xFFFFFF, 1);
}


void playing_kbd_update(void) {
  // keyboard mode enabled
  playing_update(true);
}

void playing_handle_input(uint8_t scancode) {
  extern GameMode current_mode;

  // q key to return to menu
  if (scancode == 0x10) {
    current_mode = MODE_MENU;
    return;
  }
  // p key to pause/unpause the game
  else if (scancode == 0x19) {
    current_mode = MODE_PAUSED;
    pause_init();
    return;
  }

  for (int i = 0; i < NUM_DIGLETTS; i++) {
    if (scancode == digletts[i].key) {
      // keyboard mode enabled
      playing_handle_common_input(i, true);
      break;
    }
  }
}

// Keyboard-specific initialization
void playing_kbd_init(void) {
  // keyboard mode enabled
  playing_init(true);

  for (int i = 0; i < NUM_DIGLETTS; i++) {
    digletts[i].key = diglett_keys[i];
  }
}
