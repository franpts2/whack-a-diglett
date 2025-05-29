#include "playing_mouse.h"
#include "../../../controllers/video/video.h"
#include "../../../fonts/testfont.h"
#include "playing.h"
#include <stdbool.h>
#include <stdio.h>

// Track if full redraw is needed
static bool needs_full_redraw = true;

void draw_mouse_diglett(int index) {
  extern void *back_buffer;
  extern vbe_mode_info_t m_info;

  Diglett *dig = &digletts[index];
  if (!dig->active) return;

  unsigned int color = dig->visible ? DIGLETT_COLOR : BACKGROUND_COLOR;
  unsigned int bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
  
  // Pre-calculate offsets for faster drawing
  unsigned int row_stride = m_info.XResolution * bytes_per_pixel;
  unsigned int start_offset = (dig->y * m_info.XResolution + dig->x) * bytes_per_pixel;

  // Draw to current buffer 
  void *target_buffer = get_current_buffer();
  for (int y = 0; y < dig->height; y++) {
    unsigned char* row = (unsigned char*)target_buffer + start_offset + y * row_stride;
    for (int x = 0; x < dig->width; x++) {
      *((uint32_t*)(row + x * bytes_per_pixel)) = color;
    }
  }
}

void playing_mouse_update(void) {
  bool state_changed = false;
  
  for (int i = 0; i < NUM_DIGLETTS; i++) {
    if (!digletts[i].active) continue;
    
    digletts[i].timer--;
    if (digletts[i].timer <= 0) {
      if (digletts[i].visible) {
        digletts[i].visible = false;
        visible_diglett_count--;
        digletts[i].timer = get_random_timer(MIN_DIGLETT_HIDE_TIME, MAX_DIGLETT_HIDE_TIME);
        state_changed = true;
      }
      else if (visible_diglett_count < MAX_VISIBLE_DIGLETTS) {
        digletts[i].visible = true;
        visible_diglett_count++;
        digletts[i].timer = get_random_timer(MIN_DIGLETT_SHOW_TIME, MAX_DIGLETT_SHOW_TIME);
        state_changed = true;
      }
      else {
        digletts[i].timer = get_random_timer(10, 30);
      }
    }
  }

  if (state_changed || needs_full_redraw) {
    // First draw to back buffer
    set_drawing_to_back();
    
    // Get fresh background
    copy_static_to_back();
    
    // Draw all dynamic elements
    for (int i = 0; i < NUM_DIGLETTS; i++) {
      draw_mouse_diglett(i);
    }
    draw_points_counter();

    swap_buffers();
    
    needs_full_redraw = false;
  }
}

void playing_mouse_init(void) {
  extern Cursor *g_cursor;
  playing_init(false);
  needs_full_redraw = true;
  
  if (g_cursor != NULL) {
    cursor_set_position(g_cursor, 400, 300);
  }
  set_playing_mode(false);
}

void playing_handle_mouse_input(int x, int y, bool button_pressed) {
  if (!button_pressed) return;

  for (int i = 0; i < NUM_DIGLETTS; i++) {
    if (!digletts[i].active) continue;

    if (x >= digletts[i].x && 
        x <= digletts[i].x + digletts[i].width &&
        y >= digletts[i].y && 
        y <= digletts[i].y + digletts[i].height) {
      
      playing_handle_common_input(i, false);
      needs_full_redraw = true;
      return;
    }
  }
}
