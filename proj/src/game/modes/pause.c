#include "pause.h"
#include "../../controllers/video/video.h"
#include "../../fonts/testfont.h"
#include "../modes/playing/playing.h" // Added to access BACKGROUND_COLOR
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "../background.h"

extern GameMode current_mode;
extern GameMode prev_mode;
extern int game_time_left;
static GameMode previous_game_mode;

extern struct timespec pause_start_time;
extern double total_paused_time;

extern void draw_background(void);


void pause_init(void) {
  previous_game_mode = prev_mode;
  
  // Store the time when the pause started
  clock_gettime(CLOCK_MONOTONIC, &pause_start_time);
  
  draw_pause_screen();
  
  prev_mode = MODE_PAUSED;
}

void draw_pause_screen(void) {
  set_drawing_to_back();
  
  background_draw();
  
  int title_scale = 4;
  const char *title = "PAUSED";
  int title_width = strlen(title) * 8 * title_scale;
  int title_x = (800 - title_width) / 2;
  draw_text_scaled(title, title_x, 200, 0xFFFFFF, title_scale);
  
  // Resume game
  int instruction_scale = 2;
  const char *instruction = "Press P to resume";
  int instruction_width = strlen(instruction) * 8 * instruction_scale;
  int instruction_x = (800 - instruction_width) / 2;
  draw_text_scaled(instruction, instruction_x, 300, 0xFFFFFF, instruction_scale);
  
  // Quit
  const char *menu_instruction = "Press Q to quit";
  int menu_width = strlen(menu_instruction) * 8 * instruction_scale;
  int menu_x = (800 - menu_width) / 2;
  draw_text_scaled(menu_instruction, menu_x, 350, 0xFFFFFF, instruction_scale);
  
  swap_buffers();
}

void pause_handle_input(uint8_t scancode) {
  if (scancode == 0x19) { // 'p' key to resume
    pause_resume_game();
  } else if (scancode == 0x10) { // 'q' key to quit
    current_mode = MODE_MENU;
  }
}

void pause_resume_game(void) {
  // Calculate the time spent in pause mode and add it to total_paused_time
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  double pause_duration = (now.tv_sec - pause_start_time.tv_sec) + 
                        (now.tv_nsec - pause_start_time.tv_nsec) / 1e9;
  total_paused_time += pause_duration;

  // Get access to all relevant buffers
  extern void *back_buffer;
  extern void *static_buffer;
  extern void *middle_buffer;
  unsigned int bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
  unsigned int buffer_size = m_info.XResolution * m_info.YResolution * bytes_per_pixel;
  
  // Clear ALL buffers to ensure no remnants of the pause screen remain
  memset(back_buffer, 0, buffer_size);
  memset(static_buffer, 0, buffer_size);
  if (middle_buffer) {
    memset(middle_buffer, 0, buffer_size);
  }
  
  set_drawing_to_static();
  
  vg_draw_rectangle(0, 0, 800, 600, BACKGROUND_COLOR);
  
  // Call the function to draw all Diglett holes
  draw_background();
  
  set_drawing_to_back();
  
  vg_draw_rectangle(0, 0, 800, 600, BACKGROUND_COLOR);
  
  copy_static_to_back();
  
  current_mode = MODE_PLAYING;
  prev_mode = MODE_PLAYING; 
  
  swap_buffers();
}
