#include "gameover.h"
#include "../../controllers/kbdmouse/keyboard.h"
#include "../../controllers/video/video.h"
#include "../../fonts/testfont.h"
#include "../background.h"
#include "../game.h"
#include "playing/playing_kbd.h"
#include <stdio.h>
#include "playing/playing_kbd.h"
#include "../sprites/pixelart/triangle_xpm.h"
#include "../sprites/sprite.h"
#include <string.h>

extern GameMode current_mode;
extern GameMode prev_mode;
extern int running;
extern int player_points;

extern void *back_buffer;
extern void *static_buffer;
extern void *middle_buffer;

unsigned int bytes_per_pixel;

#define NUM_BUTTONS 2

int gameover_selected = 0;
static int prev_selected = -1;

void gameover_handle_input(uint8_t scancode) {
  if (scancode == 0x48) { // Up arrow key
    gameover_selected = (gameover_selected - 1 + NUM_BUTTONS) % NUM_BUTTONS;
    prev_selected = -1;
  }
  else if (scancode == 0x50) { // Down arrow key
    gameover_selected = (gameover_selected + 1) % NUM_BUTTONS;
    prev_selected = -1;
  }
  else if (scancode == 0x1C) { // Enter key
    gameover_select_option();
  }
  else if (scancode == 0x01) { // ESC key
    current_mode = MODE_MENU;
  }
}

// Draw the background and buttons that don't require refresh
void draw_gameover_bg_and_buttons(void) {
  background_draw();

  int title_scale = 4;
  const char *title = "GAME OVER";
  int title_width = strlen(title) * 8 * title_scale;
  int title_x = (800 - title_width) / 2;
  draw_text_scaled(title, title_x, 100, 0xe27a3f, title_scale);
  
  // Draw score
  char score_text[32];
  sprintf(score_text, "Your score: %d", player_points);
  int score_scale = 2;
  int score_width = strlen(score_text) * 8 * score_scale;
  int score_x = (800 - score_width) / 2;
  draw_text_scaled(score_text, score_x, 200, 0xe27a3f, score_scale);

  int screen_w = 800;
  int btn_w = 300, btn_h = 50;
  int btn_x = (screen_w - btn_w) / 2; // Center
  int btn_y[NUM_BUTTONS] = {300, 370}; // Button positions
  const char *btn_labels[NUM_BUTTONS] = {"Play again", "Return to menu"};

  for (int i = 0; i < NUM_BUTTONS; ++i) {
    uint32_t btn_color = 0xffd789;
    vg_draw_rectangle(btn_x, btn_y[i], btn_w, btn_h, btn_color);

    int scale = 2;
    int text_width = strlen(btn_labels[i]) * 8 * scale;
    int text_x = btn_x + (btn_w - text_width) / 2;
    int text_y = btn_y[i] + (btn_h - 8 * scale) / 2;

    uint32_t text_color = 0xe27a3f;
    draw_text_scaled(btn_labels[i], text_x, text_y, text_color, scale);
  }
}

void draw_gameover_selection(void) {
  bool selection_changed = (gameover_selected != prev_selected && prev_selected >= 0 && prev_selected < NUM_BUTTONS);

  int screen_w = 800;
  int btn_w = 300;
  int btn_x = (screen_w - btn_w) / 2;
  int btn_y[NUM_BUTTONS] = {300, 370};
  int arrow_x = btn_x + btn_w + 10;
  //int arrow_w = 30, arrow_h = 50;

  static Sprite *triangle = NULL;
  
  // Only create the sprite once
  if (triangle == NULL) {
    triangle = sprite_create_from_xpm((xpm_map_t)triangle_xpm, arrow_x, btn_y[gameover_selected]);
  }

  if (selection_changed) {
    // Redraw the background on the previously selected area
    // We'll use the static buffer to get this area redrawn
    background_draw();
    
    // Redraw all buttons to ensure clarity
    int btn_h = 50;
    for (int i = 0; i < NUM_BUTTONS; ++i) {
      uint32_t btn_color = 0xffd789;
      vg_draw_rectangle(btn_x, btn_y[i], btn_w, btn_h, btn_color);
      
      const char *btn_labels[NUM_BUTTONS] = {"Play again", "Return to menu"};
      int scale = 2;
      int text_width = strlen(btn_labels[i]) * 8 * scale;
      int text_x = btn_x + (btn_w - text_width) / 2;
      int text_y = btn_y[i] + (btn_h - 8 * scale) / 2;
      
      uint32_t text_color = 0xe27a3f;
      draw_text_scaled(btn_labels[i], text_x, text_y, text_color, scale);
    }
  }

  // Position the triangle at the currently selected button
  if (triangle != NULL) {
    triangle->x = arrow_x;
    triangle->y = btn_y[gameover_selected];
    sprite_draw(triangle);
  }

  prev_selected = gameover_selected;
}

// Called once when the gameover screen is loaded
void gameover_init(void) {
  gameover_selected = 0;
  prev_selected = -1;

  extern Cursor *g_cursor;
  if (g_cursor != NULL) {
    cursor_set_position(g_cursor, 400, 300);
  }
  
  // Set prev_mode to record we're in the game over screen
  prev_mode = MODE_GAMEOVER;
}

// Called whenever we update the selection
void gameover_update_selection(void) {
  draw_gameover_selection();
}

void gameover_handle_mouse(int x, int y, bool left_button_clicked) {
  // gameover button dimensions
  int screen_w = 800;
  int btn_w = 300, btn_h = 50;
  int btn_x = (screen_w - btn_w) / 2;
  int btn_y[NUM_BUTTONS] = {300, 370};

  bool mouse_over_any_button = false;

  for (int i = 0; i < NUM_BUTTONS; i++) {
    if (x >= btn_x && x <= btn_x + btn_w &&
        y >= btn_y[i] && y <= btn_y[i] + btn_h) {
      mouse_over_any_button = true;

      if (gameover_selected != i) {
        gameover_selected = i;
        prev_selected = -1;
      }

      if (left_button_clicked) {
        gameover_select_option();
      }

      break;
    }
  }
}

void gameover_select_option(void) {
  switch (gameover_selected) {
    case 0: // Play again
      // Clear all buffers completely to remove any traces of gameover
      bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
      unsigned int buffer_size = m_info.XResolution * m_info.YResolution * bytes_per_pixel;
      memset(back_buffer, 0, buffer_size);
      memset(static_buffer, 0, buffer_size);
      memset(middle_buffer, 0, buffer_size);

      // Set to -1 to force reinitialization
      prev_mode = -1;
      current_mode = MODE_PLAYING;
      // initialize game
      playing_kbd_init();
      break;
    case 1: // Return to menu
      current_mode = MODE_MENU;
      break;
  }
}

// Main drawing function
void gameover_draw(void) {
  draw_gameover_bg_and_buttons();
  draw_gameover_selection();
}




