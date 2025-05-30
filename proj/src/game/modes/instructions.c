#include "instructions.h"
#include "../../controllers/video/video.h"
#include "../../fonts/testfont.h"
#include "../background.h"
#include "../game.h"
#include "../sprites/sprite.h"
#include "../sprites/pixelart/instructions_xpm.h"
#include <stdbool.h>
#include <string.h>

extern GameMode current_mode;
extern GameMode prev_mode;
extern void *back_buffer;
extern void *static_buffer;
extern void *middle_buffer;


// Initialize the instructions screen
void instructions_init(void) {
  // Get the bytes per pixel for the current video mode
  unsigned int bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
  unsigned int buffer_size = m_info.XResolution * m_info.YResolution * bytes_per_pixel;
  
  // Clear all buffers completely to prevent blinking with other screens
  memset(back_buffer, 0, buffer_size);
  memset(static_buffer, 0, buffer_size);
  if (middle_buffer != NULL) {
    memset(middle_buffer, 0, buffer_size);
  }
  
  // Set to drawing to static buffer for background elements
  set_drawing_to_static();
  
  // Draw the background
  background_draw();
  
 
  
  if (instructions != NULL) {
    sprite_draw(instructions, 0, 0, (uint8_t *)get_current_buffer());
  } else {
    printf("ERROR: Failed to load instructions image!\n");
  }
  
  
  // Set mode as initialized
  prev_mode = MODE_INSTRUCTIONS;
  
  // Set to drawing to back buffer for variable elements
  set_drawing_to_back();
  
  // Draw the static content to the back buffer
  copy_static_to_back();
  
  // Force an immediate buffer swap to prevent flickering
  swap_buffers();
}

// Handle keyboard input in the instructions screen
void instructions_handle_input(uint8_t scancode) {
  // Q key (0x10) to return to the menu
  if (scancode == 0x10) {
    // clean up resources
    instructions_destroy();
    
    // Set to menu mode, but let game.c handle the buffer transition
    current_mode = MODE_MENU;
    
    // Mark prev_mode as changed to force reinitialization of the menu
    prev_mode = -1;
  }
}

// cleanup resources used by the instructions screen
void instructions_destroy(void) {
  if (instructions != NULL) {
    sprite_destroy(instructions);
    instructions = NULL;
  }
}

// Draw the instructions screen
void draw_instructions(void) {
  // Copy static buffer to back buffer since we don't have dynamic content yet
  copy_static_to_back();
  
  // We're not swapping buffers here because this will be called from game.c
  // which will handle the buffer swap to maintain proper frame timing
}

