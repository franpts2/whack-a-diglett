#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include "cursor/cursor.h"

typedef enum {
  MODE_MENU,
  MODE_PLAYING,
  MODE_PAUSED,
  MODE_INSTRUCTIONS,
  MODE_GAMEOVER
} GameMode;

/** @brief Global cursor object */
extern Cursor* g_cursor;

int game_main_loop(void);
void menu_handle_input(uint8_t scancode);

#endif // GAME_H
