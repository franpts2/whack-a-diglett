#include "pause.h"
#include "../../controllers/video/video.h"
#include "../../fonts/testfont.h"
#include "../modes/playing/playing.h" // Added to access BACKGROUND_COLOR
#include <stdbool.h>
#include <string.h>
#include <time.h>

extern GameMode current_mode;
extern GameMode prev_mode;
extern int game_time_left;
static GameMode previous_game_mode;

// Add access to pause timing variables from playing.c
extern struct timespec pause_start_time;
extern double total_paused_time;

// Add declaration of the draw_background function from playing.c
extern void draw_background(void);

// Add declaration for drawing title
void draw_game_title(void) {
  // Draw title centered at the top
  int title_scale = 3;
  const char *title = "WHACK'A DIGLETT";
  int title_width = strlen(title) * 8 * title_scale;
  int title_x = (800 - title_width) / 2;
  draw_text_scaled(title, title_x, 50, 0xFFFFFF, title_scale);
}

void pause_init(void) {
  // Store the previous mode to return to it when unpausing
  previous_game_mode = prev_mode;
  
  // Store the time when the pause started
  clock_gettime(CLOCK_MONOTONIC, &pause_start_time);
  
  // Draw pause screen
  draw_pause_screen();
  
  // Mark transition as complete
  prev_mode = MODE_PAUSED;
}

void draw_pause_screen(void) {
  // Create semi-transparent overlay
  set_drawing_to_back();
  
  // Draw a semi-transparent dark overlay
  vg_draw_rectangle(0, 0, 800, 600, 0x222222);
  
  // Draw "PAUSED" text centered on screen
  int title_scale = 4;
  const char *title = "PAUSED";
  int title_width = strlen(title) * 8 * title_scale;
  int title_x = (800 - title_width) / 2;
  draw_text_scaled(title, title_x, 200, 0xFFFFFF, title_scale);
  
  // Draw instruction for resuming
  int instruction_scale = 2;
  const char *instruction = "Press P to resume";
  int instruction_width = strlen(instruction) * 8 * instruction_scale;
  int instruction_x = (800 - instruction_width) / 2;
  draw_text_scaled(instruction, instruction_x, 300, 0xFFFFFF, instruction_scale);
  
  // Draw instruction for menu
  const char *menu_instruction = "Press Q to return to menu";
  int menu_width = strlen(menu_instruction) * 8 * instruction_scale;
  int menu_x = (800 - menu_width) / 2;
  draw_text_scaled(menu_instruction, menu_x, 350, 0xFFFFFF, instruction_scale);
  
  // Swap buffers to show pause screen
  swap_buffers();
}

void pause_update(void) {
  // Nothing to update in pause mode
  // Game state is frozen until player resumes
}

void pause_handle_input(uint8_t scancode) {
  if (scancode == 0x19) { // 'p' key to resume
    pause_resume_game();
  } else if (scancode == 0x10) { // 'q' key to return to menu
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
  
  // Set to drawing to static buffer first to redraw the permanent game elements
  set_drawing_to_static();
  
  // Draw the green background to static buffer
  vg_draw_rectangle(0, 0, 800, 600, BACKGROUND_COLOR);
  
  // Draw the game title
  draw_game_title();
  
  // Call the function to draw all Diglett holes (dirt spots)
  draw_background();
  
  // Now prepare the back buffer
  set_drawing_to_back();
  
  // Draw the green background to back buffer too
  vg_draw_rectangle(0, 0, 800, 600, BACKGROUND_COLOR);
  
  // Copy static buffer to back buffer
  copy_static_to_back();
  
  // Return to the previous game mode
  current_mode = MODE_PLAYING;
  prev_mode = MODE_PLAYING; // Ensure prev_mode is also set correctly
  
  // Force a buffer swap to make the changes immediately visible
  swap_buffers();
}
