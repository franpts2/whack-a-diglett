#include "playing_mouse.h"
#include "../../../controllers/video/video.h"
#include "../../../fonts/testfont.h"
#include "playing.h"
#include <stdbool.h>
#include <stdio.h>

void draw_mouse_diglett(int index) {
  Diglett *dig = &digletts[index];
  
  if (!dig->active) return;

  // Get direct buffer access
  void *target_buffer = get_current_buffer();
  unsigned bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
  
  uint32_t color = dig->visible ? DIGLETT_COLOR : BACKGROUND_COLOR;
  uint8_t color_bytes[4];
  
  // Pre-calculate color bytes
  for (unsigned i = 0; i < bytes_per_pixel; i++) {
    color_bytes[i] = (color >> (i * 8)) & 0xFF;
  }

  // Draw directly to buffer
  for (int row = 0; row < dig->height; row++) {
    int y_pos = dig->y + row;
    if (y_pos < 0 || y_pos >= m_info.YResolution) continue;
    
    unsigned int start_pos = (y_pos * m_info.XResolution + dig->x) * bytes_per_pixel;
    
    for (int col = 0; col < dig->width; col++) {
      int x_pos = dig->x + col;
      if (x_pos < 0 || x_pos >= m_info.XResolution) continue;
      
      unsigned int current_pos = start_pos + (col * bytes_per_pixel);
      memcpy((uint8_t *)target_buffer + current_pos, color_bytes, bytes_per_pixel);
    }
  }
}

void playing_mouse_update(void) {
  static int update_count = 0;
  update_count++;
  
  // Only copy static background every few frames to reduce memory operations
  if (update_count % 3 == 0) {
    set_drawing_to_back();
    copy_static_to_back();
  }
  
  // Update game state and draw digletts
  for (int i = 0; i < NUM_DIGLETTS; i++) {
    if (!digletts[i].active) continue;
    
    digletts[i].timer--;
    
    if (digletts[i].timer <= 0) {
      if (digletts[i].visible) {
        digletts[i].visible = false;
        visible_diglett_count--;
        digletts[i].timer = get_random_timer(MIN_DIGLETT_HIDE_TIME, MAX_DIGLETT_HIDE_TIME);
      }
      else if (visible_diglett_count < MAX_VISIBLE_DIGLETTS) {
        digletts[i].visible = true;
        visible_diglett_count++;
        digletts[i].timer = get_random_timer(MIN_DIGLETT_SHOW_TIME, MAX_DIGLETT_SHOW_TIME);
      }
      else {
        digletts[i].timer = get_random_timer(10, 30);
      }
    }
    draw_mouse_diglett(i);
  }
  
  draw_points_counter();
}

// Initialize the mouse playing mode
void playing_mouse_init(void) {
  extern Cursor *g_cursor;

  playing_init(false);

  if (g_cursor != NULL) {
    cursor_set_position(g_cursor, 400, 300);
  }

  set_playing_mode(false);
}

// Handle mouse input during gameplay
void playing_handle_mouse_input(int x, int y, bool button_pressed) {
  if (!button_pressed) {
    return;
  }
  
  for (int i = 0; i < NUM_DIGLETTS; i++) {
    if (!digletts[i].active) {
      continue;
    }
    
    if (x >= digletts[i].x && 
        x <= digletts[i].x + digletts[i].width &&
        y >= digletts[i].y && 
        y <= digletts[i].y + digletts[i].height) {
      
      playing_handle_common_input(i, false);
      return;
    }
  }
}
