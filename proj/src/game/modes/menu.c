#include "menu.h"
#include "../../controllers/kbdmouse/keyboard.h"
#include "../../controllers/video/video.h"
#include "../../fonts/testfont.h"
#include "../game.h" // Includes GameMode definition

// External variables for game state control
extern GameMode current_mode;
extern GameMode prev_mode;
extern int running; // Controls main game loop in game.c

#define MENU_ITEMS 3

int selected = 0;              // Botão selecionado. 0 - Start Game, 1 - Instructions, 2 - Exit
static int prev_selected = -1; // -1 ensures initial drawing

void menu_handle_input(uint8_t scancode) { // only change selection on arrow key presses
  if (scancode == 0x48) {                  // Up arrow key
    selected = (selected - 1 + MENU_ITEMS) % MENU_ITEMS;

    // force a selection update in the game loop
    prev_selected = -1; // forces redraw of selection
  }
  else if (scancode == 0x50) { // Down arrow key
    selected = (selected + 1) % MENU_ITEMS;

    prev_selected = -1;
  }
  else if (scancode == 0x1C) { // Enter key
    switch (selected) {
      case 0: // "Start Game" button
        current_mode = MODE_PLAYING;
        break;
      case 1: // "Instructions" button
        current_mode = MODE_INSTRUCTIONS;
        break;
      case 2:        // "Exit" button
        running = 0; 
        break;
    }
  }
}

// Desenha as coisas que não precisam de refresh (botões fundo etc)
void draw_menu_bg_and_buttons(void) {
  vg_draw_rectangle(0, 0, 800, 600, 0x02);

  int screen_w = 800;
  int btn_w = 300, btn_h = 50;
  int btn_x = (screen_w - btn_w) / 2; // Centrar
  int btn_y[MENU_ITEMS] = {250, 320, 390};
  const char *btn_labels[MENU_ITEMS] = {"Start Game", "Instructions", "Exit"};

  for (int i = 0; i < MENU_ITEMS; ++i) {
    // Use a lighter color for the currently selected button
    uint32_t btn_color = (i == selected) ? 0xCCCCCC : 0xAAAAAA;
    vg_draw_rectangle(btn_x, btn_y[i], btn_w, btn_h, btn_color);

    int scale = 2;
    int text_width = strlen(btn_labels[i]) * 8 * scale;
    int text_x = btn_x + (btn_w - text_width) / 2;
    int text_y = btn_y[i] + (btn_h - 8 * scale) / 2;

    // Use a different text color for selected items
    uint32_t text_color = (i == selected) ? 0x0000AA : 0x02; // Darker blue for selected text
    draw_text_scaled(btn_labels[i], text_x, text_y, text_color, scale);
  }
}

// Desenha o retangulo de seleção pq n sei fazer triangulos (skull emoji)
void draw_menu_selection(void) {
  // always draw selection indicator, only erase the old selection if we're changing positions
  bool selection_changed = (selected != prev_selected && prev_selected >= 0 && prev_selected < MENU_ITEMS);

  int screen_w = 800;
  int btn_w = 300;
  int btn_x = (screen_w - btn_w) / 2;
  int btn_y[MENU_ITEMS] = {250, 320, 390};
  int arrow_x = btn_x + btn_w + 10;
  int arrow_w = 30, arrow_h = 50;

  // direct access to buffer for faster drawing
  void *target_buffer = get_current_buffer();
  unsigned bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;

  // color bytes for direct buffer access
  uint8_t bg_color_bytes[4] = {0};  // Background color (0x02)
  uint8_t sel_color_bytes[4] = {0}; // Selection color (0x0000FF) - blue

  // set background color (green)
  bg_color_bytes[0] = 0x02; // G component in RGB

  // Set selection color (bright blue)
  sel_color_bytes[0] = 0xFF; // B component in RGB

  // Only erase previous selection if selection changed
  if (selection_changed) {
    for (int y = btn_y[prev_selected]; y < btn_y[prev_selected] + arrow_h; y++) {
      if (y < 0 || y >= m_info.YResolution)
        continue;

      for (int x = arrow_x; x < arrow_x + arrow_w; x++) {
        if (x < 0 || x >= m_info.XResolution)
          continue;

        unsigned int pixel_pos = (y * m_info.XResolution + x) * bytes_per_pixel;
        for (unsigned i = 0; i < bytes_per_pixel; i++) {
          *((uint8_t *) target_buffer + pixel_pos + i) = bg_color_bytes[i];
        }
      }
    }
  }

  // Always draw the current selection with optimized direct buffer access
  for (int y = btn_y[selected]; y < btn_y[selected] + arrow_h; y++) {
    if (y < 0 || y >= m_info.YResolution)
      continue;

    for (int x = arrow_x; x < arrow_x + arrow_w; x++) {
      if (x < 0 || x >= m_info.XResolution)
        continue;

      unsigned int pixel_pos = (y * m_info.XResolution + x) * bytes_per_pixel;
      for (unsigned i = 0; i < bytes_per_pixel; i++) {
        *((uint8_t *) target_buffer + pixel_pos + i) = sel_color_bytes[i];
      }
    }
  }

  prev_selected = selected;
}

// Chamado 1 vez quando o menu é loaded
void menu_init(void) {
  selected = 0;       // reset selection to first item
  prev_selected = -1; // force redraw of selection on first display
}

// Chamado sempre que atualizarmos o selected
void menu_update_selection(void) {
  draw_menu_selection();
}

// Handles mouse position and clicks for menu buttons
void menu_handle_mouse(int x, int y, bool left_button_clicked) {
  // Menu button dimensions
  int screen_w = 800;
  int btn_w = 300, btn_h = 50;
  int btn_x = (screen_w - btn_w) / 2;
  int btn_y[MENU_ITEMS] = {250, 320, 390};

  bool mouse_over_any_button = false;

  // Check if mouse is over any button
  for (int i = 0; i < MENU_ITEMS; i++) {
    if (x >= btn_x && x <= btn_x + btn_w &&
        y >= btn_y[i] && y <= btn_y[i] + btn_h) {
      // Mouse is over this button
      mouse_over_any_button = true;

      if (selected != i) {
        // Update selection if it changed
        selected = i;
        // Force redraw by setting prev_selected to -1
        prev_selected = -1;
      }

      // If the mouse button was clicked while over a button
      if (left_button_clicked) {
        // Perform action based on which button was clicked
        switch (i) {
          case 0: // "Start Game" button
            current_mode = MODE_PLAYING;
            break;
          case 1: // "Instructions" button
            current_mode = MODE_INSTRUCTIONS;
            break;
          case 2: // "Exit" button
            // Signal to exit the game
            running = 0;
            break;
        }
      }

      // Exit after finding the right button
      break;
    }
  }

  // If mouse wasn't over any button, don't change selection
  // This allows the selection to stay where it was last set
}
