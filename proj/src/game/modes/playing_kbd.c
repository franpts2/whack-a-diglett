#include "playing_kbd.h"
#include "../../controllers/kbdmouse/keyboard.h"
#include "../../controllers/video/video.h"
#include "../../fonts/testfont.h"
#include "playing.h"
#include <stdbool.h>
#include <stdio.h>

// scancodes for the keys assigned to digletts
const uint8_t diglett_keys[NUM_DIGLETTS] = {
  0x13, 0x14, 0x15, // r, t, y (first row)
  0x21, 0x22, 0x23, // f, g, h (second row)
  0x2F, 0x30, 0x31  // v, b, n (third row)
};

// Keyboard-specific drawing function for digletts with key labels
void draw_kbd_diglett(int index) {
  Diglett *dig = &digletts[index];

  if (dig->visible) {
    // First call the common drawing code
    draw_diglett(index);

    // Then add keyboard-specific labels
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
}

// Keyboard-specific version of the update function
void playing_kbd_update(void) {
  bool needs_update = false;

  // First, copy static background to back buffer
  set_drawing_to_back();
  copy_static_to_back();

  // Process all digletts using common game logic
  for (int i = 0; i < NUM_DIGLETTS; i++) {
    if (!digletts[i].active)
      continue; // skip inactive digletts

    digletts[i].timer--;

    if (digletts[i].timer <= 0) {
      // change state
      if (digletts[i].visible) {
        // currently visible diglett should hide
        digletts[i].visible = false;
        visible_diglett_count--;

        // timer for hidden state
        digletts[i].timer = get_random_timer(MIN_DIGLETT_HIDE_TIME, MAX_DIGLETT_HIDE_TIME);
        needs_update = true;
      }
      else if (visible_diglett_count < MAX_VISIBLE_DIGLETTS) {
        digletts[i].visible = true;
        visible_diglett_count++;

        // timer for visible state
        digletts[i].timer = get_random_timer(MIN_DIGLETT_SHOW_TIME, MAX_DIGLETT_SHOW_TIME);
        needs_update = true;
      }
      else {
        // at max visible capacity, reset timer and try again later
        digletts[i].timer = get_random_timer(10, 30);
      }
    }
  }

  // Draw all visible digletts with keyboard labels
  for (int i = 0; i < NUM_DIGLETTS; i++) {
    if (digletts[i].active && digletts[i].visible) {
      draw_kbd_diglett(i);
    }
  }

  // Update points display
  draw_points_counter();
}

// handles keyboard input in playing mode
// Keyboard-specific input handling
void playing_handle_input(uint8_t scancode) {
  extern GameMode current_mode;

  // q key to return to menu
  if (scancode == 0x10) {
    current_mode = MODE_MENU;
    return;
  }

  // Make sure we're drawing to back buffer
  set_drawing_to_back();
  copy_static_to_back();

  for (int i = 0; i < NUM_DIGLETTS; i++) {
    if (scancode == digletts[i].key) {
      // Whack logic - visible digletts give points, hidden cost points
      if (digletts[i].visible) {
        // diglett foi whacked :P
        digletts[i].visible = false;
        visible_diglett_count--;

        // new timer for this diglett to reappear
        digletts[i].timer = get_random_timer(MIN_DIGLETT_HIDE_TIME, MAX_DIGLETT_HIDE_TIME);

        // +1 point
        if (player_points < 999) {
          player_points++;
        }
      }
      else {
        // -1 point
        if (player_points > 0) {
          player_points--;
        }
      }

      // redraw all game elements after state change
      playing_kbd_update();
      break;
    }
  }
}

// Keyboard-specific initialization - should be called before common playing_init
void playing_kbd_init(void) {
  // Assign specific keyboard keys to each diglett
  for (int i = 0; i < NUM_DIGLETTS; i++) {
    digletts[i].key = diglett_keys[i];
  }

  // Call the common initialization
  playing_init();
}
