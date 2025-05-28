#include "playing_mouse.h"
#include "../../../controllers/video/video.h"
#include "../../../fonts/testfont.h"
#include "playing.h"
#include <stdbool.h>
#include <stdio.h>

void draw_mouse_diglett(int index) {
  draw_diglett(index, false);
}

void playing_mouse_update(void) {
  extern void *back_buffer;
  extern void *static_buffer;
 
  void *current = get_current_buffer();
  if (current == back_buffer) {
    copy_static_to_back();
  }
  
  set_drawing_to_back();
  
  playing_update(false);
}

// Initialize the mouse playing mode
void playing_mouse_init(void) {
  extern Cursor *g_cursor;
  extern int mode_selected;
  extern void *static_buffer;
  extern void *back_buffer;
  extern void *middle_buffer;
  
  // start with a clean state
  unsigned int bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
  unsigned int buffer_size = m_info.XResolution * m_info.YResolution * bytes_per_pixel;
  memset(back_buffer, 0, buffer_size);
  memset(middle_buffer, 0, buffer_size);
  memset(static_buffer, 0, buffer_size);
  
  // Position cursor in the center initially
  if (g_cursor != NULL && mode_selected == 1) {
    cursor_set_position(g_cursor, 400, 300);
  }
  
  playing_init(false);
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
