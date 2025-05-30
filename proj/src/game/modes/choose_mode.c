#include "choose_mode.h"
#include "../../controllers/kbdmouse/keyboard.h"
#include "../../controllers/video/video.h"
#include "../../fonts/testfont.h"
#include "../game.h"
#include "playing-modes/playing_kbd.h"
#include "playing-modes/playing_mouse.h"

extern GameMode current_mode;
extern GameMode prev_mode;
extern int running;

extern void *back_buffer;
extern void *static_buffer;
extern void *middle_buffer;

unsigned int choose_mode_bytes_per_pixel;

#define CHOOSE_MODE_ITEMS 2

int mode_selected = 0;
static int prev_mode_selected = -1;

void choose_mode_handle_input(uint8_t scancode) {
  if (scancode == 0x48) { // Up arrow key
    mode_selected = (mode_selected - 1 + CHOOSE_MODE_ITEMS) % CHOOSE_MODE_ITEMS;
    prev_mode_selected = -1;
  }
  else if (scancode == 0x50) { // Down arrow key
    mode_selected = (mode_selected + 1) % CHOOSE_MODE_ITEMS;
    prev_mode_selected = -1;
  }
  else if (scancode == 0x1C) { // Enter key
    choose_mode_select_option();
  }
  else if (scancode == 0x10) { // Q key
    current_mode = MODE_MENU;
  }
}

void draw_choose_mode_bg_and_buttons(void) {
  vg_draw_rectangle(0, 0, 800, 600, BACKGROUND_COLOR);

  // title centrado
  int title_scale = 3;
  const char *title = "CHOOSE MODE";
  int title_width = strlen(title) * 8 * title_scale;
  int title_x = (800 - title_width) / 2;
  draw_text_scaled(title, title_x, 100, 0xFF, title_scale);

  // Subtitle
  const char *subtitle = "Select how you want to play:";
  int subtitle_scale = 2;
  int subtitle_width = strlen(subtitle) * 8 * subtitle_scale;
  int subtitle_x = (800 - subtitle_width) / 2;
  draw_text_scaled(subtitle, subtitle_x, 180, 0xFFFF, subtitle_scale);

  // Draw the two option buttons
  int screen_w = 800;
  int btn_w = 300, btn_h = 50;
  int btn_x = (screen_w - btn_w) / 2;
  int btn_y[CHOOSE_MODE_ITEMS] = {250, 320};
  const char *btn_labels[CHOOSE_MODE_ITEMS] = {"Keyboard", "Mouse"};

  for (int i = 0; i < CHOOSE_MODE_ITEMS; ++i) {
    uint32_t btn_color = 0xAAAAAA;
    vg_draw_rectangle(btn_x, btn_y[i], btn_w, btn_h, btn_color);

    int scale = 2;
    int text_width = strlen(btn_labels[i]) * 8 * scale;
    int text_x = btn_x + (btn_w - text_width) / 2;
    int text_y = btn_y[i] + (btn_h - 8 * scale) / 2;

    uint32_t text_color = 0x04;
    draw_text_scaled(btn_labels[i], text_x, text_y, text_color, scale);
  }

  const char *back_msg = "Press Q to go back";
  int back_scale = 1;
  int back_width = strlen(back_msg) * 8 * back_scale;
  int back_x = (800 - back_width) / 2;
  draw_text_scaled(back_msg, back_x, 500, 0xFFFF, back_scale);
}

void draw_choose_mode_selection(void) {
  bool selection_changed = (mode_selected != prev_mode_selected && prev_mode_selected >= 0 && prev_mode_selected < CHOOSE_MODE_ITEMS);

  int screen_w = 800;
  int btn_w = 300;
  int btn_x = (screen_w - btn_w) / 2;
  int btn_y[CHOOSE_MODE_ITEMS] = {250, 320};
  int arrow_x = btn_x + btn_w + 10;
  int arrow_w = 30, arrow_h = 50;

  void *target_buffer = get_current_buffer();
  choose_mode_bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;

  uint8_t bg_color_bytes[4] = {0};  // Background color (0x04)
  uint8_t sel_color_bytes[4] = {0}; // Selection color (0x0000FF) - blue

  bg_color_bytes[0] = 0x04;
  sel_color_bytes[0] = 0xFF;

  if (selection_changed) {
    for (int y = btn_y[prev_mode_selected]; y < btn_y[prev_mode_selected] + arrow_h; y++) {
      if (y < 0 || y >= m_info.YResolution)
        continue;

      for (int x = arrow_x; x < arrow_x + arrow_w; x++) {
        if (x < 0 || x >= m_info.XResolution)
          continue;

        unsigned int pixel_pos = (y * m_info.XResolution + x) * choose_mode_bytes_per_pixel;
        for (unsigned i = 0; i < choose_mode_bytes_per_pixel; i++) {
          *((uint8_t *) target_buffer + pixel_pos + i) = bg_color_bytes[i];
        }
      }
    }
  }

  for (int y = btn_y[mode_selected]; y < btn_y[mode_selected] + arrow_h; y++) {
    if (y < 0 || y >= m_info.YResolution)
      continue;

    for (int x = arrow_x; x < arrow_x + arrow_w; x++) {
      if (x < 0 || x >= m_info.XResolution)
        continue;

      unsigned int pixel_pos = (y * m_info.XResolution + x) * choose_mode_bytes_per_pixel;
      for (unsigned i = 0; i < choose_mode_bytes_per_pixel; i++) {
        *((uint8_t *) target_buffer + pixel_pos + i) = sel_color_bytes[i];
      }
    }
  }

  prev_mode_selected = mode_selected;
}

void choose_mode_init(void) {
  mode_selected = 0;
  prev_mode_selected = -1;

  // cursor position to center of screen
  extern Cursor *g_cursor;
  if (g_cursor != NULL) {
    cursor_set_position(g_cursor, 400, 300);
  }
}

void choose_mode_update_selection(void) {
  draw_choose_mode_selection();
}

void choose_mode_handle_mouse(int x, int y, bool left_button_clicked) {
  int screen_w = 800;
  int btn_w = 300, btn_h = 50;
  int btn_x = (screen_w - btn_w) / 2;
  int btn_y[CHOOSE_MODE_ITEMS] = {250, 320};

  bool mouse_over_any_button = false;

  for (int i = 0; i < CHOOSE_MODE_ITEMS; i++) {
    if (x >= btn_x && x <= btn_x + btn_w &&
        y >= btn_y[i] && y <= btn_y[i] + btn_h) {
      mouse_over_any_button = true;

      if (mode_selected != i) {
        mode_selected = i;
        prev_mode_selected = -1;
      }

      if (left_button_clicked) {
        choose_mode_select_option();
      }

      break;
    }
  }
}

void choose_mode_select_option(void) {
  choose_mode_bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
  unsigned int buffer_size = m_info.XResolution * m_info.YResolution * choose_mode_bytes_per_pixel;

  // clear all buffers
  memset(back_buffer, 0, buffer_size);
  memset(static_buffer, 0, buffer_size);
  memset(middle_buffer, 0, buffer_size);

  prev_mode = -1;

  current_mode = MODE_PLAYING;

  switch (mode_selected) {
    case 0: // kbd mode
      playing_kbd_init();
      break;
    case 1: // mouse mode
      playing_mouse_init();
      break;
  }
}
