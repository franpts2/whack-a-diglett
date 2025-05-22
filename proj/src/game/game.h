#ifndef GAME_H
#define GAME_H

#include <stdint.h>

typedef enum {
  MODE_MENU,
  MODE_PLAYING,
  MODE_PAUSED,
  MODE_INSTRUCTIONS,
  MODE_GAMEOVER
} GameMode;

int game_main_loop(void);
void menu_handle_input(uint8_t scancode);

#endif // GAME_H
