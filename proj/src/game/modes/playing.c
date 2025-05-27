#include "playing.h"
#include "../../controllers/kbdmouse/keyboard.h"
#include "../../controllers/video/video.h"
#include "../../fonts/testfont.h"
#include "../../game/game.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Array to store all digletts

int visible_diglett_count = 0;
int player_points = 0;

// scancodes for the keys assigned to digletts
const uint8_t diglett_keys[NUM_DIGLETTS] = {
  0x13, 0x14, 0x15, // r, t, y (first row)
  0x21, 0x22, 0x23, // f, g, h (second row)
  0x2F, 0x30, 0x31  // v, b, n (third row)
};

// helper function to get a random timer value
int get_random_timer(int min, int max) {
  return min + (rand() % (max - min + 1));
}

// Function to draw the points counter
void draw_points_counter() {
  // caclcular posição pro counter de pontos nao sair da tela
  int counter_width = 200;
  int counter_x = 800 - counter_width - 10;

  // width needed for "Points: " (7 chars * 8 pixels * 2 scale)
  int points_label_width = 8 * 8 * 2;

  // position for the points number
  int number_area_x = counter_x + 10 + points_label_width;

  // don't exceed screen boundaries
  int max_number_width = counter_width - points_label_width - 20;

  // clear the number area
  vg_draw_rectangle(number_area_x, 10, max_number_width, 30, BACKGROUND_COLOR);

  char points_str[20] = {0};

  int safe_points = player_points;
  if (safe_points < 0)
    safe_points = 0;
  if (safe_points > 999)
    safe_points = 999;

  sprintf(points_str, "%d", safe_points);

  draw_text_scaled(points_str, number_area_x, 10, 0xFFFFFF, 2);
}

// initialize the playing screen
void playing_init(void) {
  // seed the random number generator
  srand(time(NULL));

  visible_diglett_count = 0;
  player_points = 0;

  extern GameMode prev_mode;
  extern Cursor *g_cursor;

  // hide cursor in playing mode by moving it off-screen (it won't be rendered anyway)
  if (g_cursor != NULL) {
    cursor_set_position(g_cursor, -100, -100);
  }

  prev_mode = MODE_PLAYING; // Mark transition as complete

  // Reset the screen completely - this is crucial to remove menu artifacts
  extern void *back_buffer;
  extern void *static_buffer;
  extern void *middle_buffer;
  unsigned int bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
  unsigned int buffer_size = m_info.XResolution * m_info.YResolution * bytes_per_pixel;

  // Clear all buffers first
  memset(back_buffer, 0, buffer_size);
  memset(middle_buffer, 0, buffer_size);
  memset(static_buffer, 0, buffer_size);

  // Set to draw to static buffer for the game background - IMPORTANT
  set_drawing_to_static();

  // Draw static background elements (these won't change during gameplay)
  vg_draw_rectangle(0, 0, 800, 600, BACKGROUND_COLOR);

  // titulo centrado
  int title_scale = 3;
  const char *title = "WHACK'A DIGLETT";
  int title_width = strlen(title) * 8 * title_scale;
  int title_x = (800 - title_width) / 2;
  draw_text_scaled(title, title_x, 50, 0xFFFFFF, title_scale);

  int rect_width = 60;
  int rect_height = 80;
  int spacing = 60; // space between digletts

  int grid_width = 3 * rect_width + 2 * spacing;
  // int grid_height = 3 * rect_height + 2 * spacing;

  // calculate starting position to center the grid
  int start_x = (800 - grid_width) / 2;
  int start_y = 150;

  // 3x3 grid - draw holes to static buffer
  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
      int index = row * 3 + col;
      int x = start_x + col * (rect_width + spacing);
      int y = start_y + row * (rect_height + spacing);

      digletts[index].x = x;
      digletts[index].y = y;
      digletts[index].width = rect_width;
      digletts[index].height = rect_height;
      digletts[index].key = diglett_keys[index];
      digletts[index].visible = false; // start with all digletts hidden
      digletts[index].active = true;
      digletts[index].timer = get_random_timer(MIN_DIGLETT_HIDE_TIME, MAX_DIGLETT_HIDE_TIME);

      // Draw empty hole to static buffer
      vg_draw_rectangle(x, y, rect_width, rect_height, BACKGROUND_COLOR);
    }
  }

  int counter_width = 200;
  int counter_x = 800 - counter_width - 10;

  // clear the entire counter area
  vg_draw_rectangle(counter_x, 10, counter_width, 30, BACKGROUND_COLOR);

  // Draw static "Points:" label
  draw_text_scaled("Points:", counter_x + 10, 10, 0xFFFFFF, 2);

  // Now switch back to back buffer for dynamic elements
  set_drawing_to_back();

  // Copy static background to back buffer for initial display
  copy_static_to_back();

  // draw initial points value (0) directly to back buffer
  draw_points_counter();
}

// handles keyboard input in playing mode
void playing_handle_input(uint8_t scancode) {
  extern GameMode current_mode;

  // q key to returns to menu
  if (scancode == 0x10) {
    current_mode = MODE_MENU;
    return;
  }

  // Make sure we're drawing to back buffer
  set_drawing_to_back();
  copy_static_to_back();

  for (int i = 0; i < NUM_DIGLETTS; i++) {
    if (scancode == digletts[i].key) {

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

      // Redraw all game elements after state change
      playing_update();
      break;
    }
  }
}

// show or hide DIGLETT
void update_diglett_visibility(int index) {
  // Make sure we're drawing to the back buffer
  set_drawing_to_back();

  // Get static content before drawing dynamic elements
  copy_static_to_back();

  Diglett *dig = &digletts[index];

  if (dig->visible) {
    // Draw diglett
    vg_draw_rectangle(dig->x, dig->y, dig->width, dig->height, DIGLETT_COLOR);

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
  else {
    // Just restore background (hole) from static buffer
    // This is already done by copy_static_to_back() above
  }
}

// update game state
void playing_update(void) {
  bool needs_update = false;

  // First, copy static background to back buffer
  set_drawing_to_back();
  copy_static_to_back();

  // Process all digletts
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

  // Draw all visible digletts
  for (int i = 0; i < NUM_DIGLETTS; i++) {
    if (digletts[i].active && digletts[i].visible) {
      Diglett *dig = &digletts[i];
      // Draw diglett
      vg_draw_rectangle(dig->x, dig->y, dig->width, dig->height, DIGLETT_COLOR);

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

  // Update points display
  draw_points_counter();
}
