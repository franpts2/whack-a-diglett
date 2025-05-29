#include "pause.h"
#include "../../controllers/video/video.h"
#include "../../fonts/testfont.h"
#include <stdbool.h>
#include <string.h>

extern GameMode current_mode;
extern GameMode prev_mode;
extern int game_time_left;
static GameMode previous_game_mode;

void pause_init(void) {
  // Store the previous mode to return to it when unpausing
  previous_game_mode = prev_mode;
  
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
  // Return to the previous game mode (should be MODE_PLAYING)
  current_mode = previous_game_mode;
}
