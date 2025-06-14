#include "menu.h"
#include "../../controllers/kbdmouse/keyboard.h"
#include "../../controllers/video/video.h"
#include "../../fonts/testfont.h"
#include "../background.h"
#include "../game.h"
#include "playing/playing_kbd.h"
#include "../sprites/pixelart/triangle_xpm.h"
#include "../sprites/sprite.h"


extern GameMode current_mode;
extern GameMode prev_mode;
extern int running;

extern void *back_buffer;
extern void *static_buffer;
extern void *middle_buffer;

unsigned int bytes_per_pixel;

#define MENU_ITEMS 3

int selected = 0;
static int prev_selected = -1;

void menu_handle_input(uint8_t scancode) {
  if (scancode == 0x48) { // Up arrow key
    selected = (selected - 1 + MENU_ITEMS) % MENU_ITEMS;
    prev_selected = -1;
  }
  else if (scancode == 0x50) { // Down arrow key
    selected = (selected + 1) % MENU_ITEMS;
    prev_selected = -1;

  }
  else if (scancode == 0x1C) { // Enter key
    menu_select_option();
  }
}

void draw_menu_bg_and_buttons(void) {
  background_draw();

  int title_scale = 4;
  const char *title = "WHACK'A DIGLETT";
  int title_width = strlen(title) * 8 * title_scale;
  int title_x = (800 - title_width) / 2;
  draw_text_scaled(title, title_x, 100, 0xe27a3f, title_scale);

  int screen_w = 800;
  int btn_w = 300, btn_h = 50;
  int btn_x = (screen_w - btn_w) / 2;
  int btn_y[MENU_ITEMS] = {250, 320, 390};
  const char *btn_labels[MENU_ITEMS] = {"Start Game", "Instructions", "Exit"};

  for (int i = 0; i < MENU_ITEMS; ++i) {

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


void draw_menu_selection(void) {
  bool selection_changed = (selected != prev_selected && prev_selected >= 0 && prev_selected < MENU_ITEMS);

  int screen_w = 800;
  int btn_w = 300;
  int btn_x = (screen_w - btn_w) / 2;
  int btn_y[MENU_ITEMS] = {250, 320, 390};
  int arrow_x = btn_x + btn_w + 10;
  //int arrow_w = 30, arrow_h = 50;
  
  static Sprite *triangle = NULL;
  
  // Only create the sprite once
  if (triangle == NULL) {
    triangle = sprite_create_from_xpm((xpm_map_t)triangle_xpm, arrow_x, btn_y[selected]);
  }
  
  // Clear the previous selection if needed
  if (selection_changed) {
    // Redraw the background on the previously selected area
    // We'll use the static buffer to get this area redrawn
    background_draw();
    
    // Redraw all buttons to ensure clarity
    int btn_h = 50;
    for (int i = 0; i < MENU_ITEMS; ++i) {
      uint32_t btn_color = 0xffd789;
      vg_draw_rectangle(btn_x, btn_y[i], btn_w, btn_h, btn_color);
      
      const char *btn_labels[MENU_ITEMS] = {"Start Game", "Instructions", "Exit"};
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
    triangle->y = btn_y[selected];
    sprite_draw(triangle);
  }
  
  prev_selected = selected;
}

void menu_init(void) {
  selected = 0;
  prev_selected = -1;

  extern Cursor *g_cursor;
  if (g_cursor != NULL) {
    cursor_set_position(g_cursor, 400, 300);
  }
}

void menu_update_selection(void) {
  draw_menu_selection();
}

void menu_handle_mouse(int x, int y, bool left_button_clicked) {
  int screen_w = 800;
  int btn_w = 300, btn_h = 50;
  int btn_x = (screen_w - btn_w) / 2;
  int btn_y[MENU_ITEMS] = {250, 320, 390};

  bool mouse_over_any_button = false;

  for (int i = 0; i < MENU_ITEMS; i++) {
    if (x >= btn_x && x <= btn_x + btn_w &&
        y >= btn_y[i] && y <= btn_y[i] + btn_h) {
      mouse_over_any_button = true;

      if (selected != i) {
        selected = i;
        prev_selected = -1;
      }

      if (left_button_clicked) {
        menu_select_option();
      }

      break;
    }
  }
}


void menu_select_option(void) {
  
  switch (selected) {
    case 0: // Start Game
      // First force a clean state for all buffers
      bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
      unsigned int buffer_size = m_info.XResolution * m_info.YResolution * bytes_per_pixel;

      // Clear all buffers completely to remove any traces of menu
      memset(back_buffer, 0, buffer_size);
      memset(static_buffer, 0, buffer_size);
      memset(middle_buffer, 0, buffer_size);

      // Set to -1 to force reinitialization
      prev_mode = -1;

      current_mode = MODE_PLAYING;
      playing_kbd_init();
      break;
    case 1: // Instructions
      current_mode = MODE_INSTRUCTIONS;
      break;
    case 2: // Exit
      running = false;
      break;
  }
}
