#include "controllers/kbdmouse/aux.h"
#include "controllers/kbdmouse/keyboard.h"
#include "controllers/timer/timer.h"
#include "controllers/video/video.h"
#include "game/game.h"
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

int(proj_main_loop)(int argc, char *argv[]) {
  // Set up do video
  uint16_t mode = 0x115; // Changed from 0x105 to 0x115 for 800x600 resolution with 24-bit color (8:8:8)
  if (map_frame_buffer(mode) != 0) {
    printf("Failed to map frame buffer!\n");
    return 1;
  }
  if (set_video_mode(mode) != 0) {
    printf("Failed to set video mode!\n");
    return 1;
  }

  // Começar o game loop
  int ret = game_main_loop();

  return ret;
}

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  if (lcf_start(argc, argv))
    return 1;

  lcf_cleanup();
  return 0;
}
