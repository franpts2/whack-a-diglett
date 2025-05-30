/**
 * @file main.c
 * @brief Main program entry point
 *
 * This file contains the program's entry point and initializes 
 * the video mode before launching the main game loop.
 */

#include "controllers/kbdmouse/aux.h"
#include "controllers/kbdmouse/keyboard.h"
#include "controllers/timer/timer.h"
#include "controllers/video/video.h"
#include "game/game.h"
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Project's main loop
 *
 * Initializes video mode and runs the game loop.
 * Cleans up resources before exiting.
 *
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return 0 upon success, non-zero otherwise
 */
int(proj_main_loop)(int argc, char *argv[]) {
  // Set up video mode
  uint16_t mode = 0x115;
  if (map_frame_buffer(mode) != 0) {
    printf("Failed to map frame buffer!\n");
    return 1;
  }
  if (set_video_mode(mode) != 0) {
    printf("Failed to set video mode!\n");
    return 1;
  }
  
  // Start the game loop
  int ret = game_main_loop();
  
  // Clean up and return to text mode
  vg_exit();
  return ret;
}

/**
 * @brief Program entry point
 *
 * Sets up LCF library and starts the project's main loop.
 * Performs cleanup before exiting.
 *
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return 0 upon success, non-zero otherwise
 */
int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");
  lcf_log_output("/home/lcom/labs/proj/output.txt");
  if (lcf_start(argc, argv))
    return 1;
  lcf_cleanup();
  return 0;
}
