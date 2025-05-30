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
  uint16_t mode = 0x115;
  if (map_frame_buffer(mode) != 0) {
    printf("Failed to map frame buffer!\n");
    return 1;
  }
  if (set_video_mode(mode) != 0) {
    printf("Failed to set video mode!\n");
    return 1;
  }
  if (load_sprites() != 0) return 1;
  // Start the game loop
  int ret = game_main_loop();

  // Clean up and return to text mode
  vg_exit();

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
