/**
 * @file game.h
 * @brief Core game header file
 *
 * Contains the main game modes, global variables, and core functions.
 */

#ifndef GAME_H
#define GAME_H

#include "cursor/cursor.h"
#include <stdint.h>

/**
 * @brief Enumeration of game modes
 * 
 * Defines the different states in which the game can be.
 */
typedef enum {
  MODE_MENU,         /**< Main menu mode */
  MODE_PLAYING,      /**< Active gameplay mode */
  MODE_PAUSED,       /**< Game paused mode */
  MODE_INSTRUCTIONS, /**< Game instructions mode */
  MODE_GAMEOVER      /**< Game over mode */
} GameMode;

/** @brief Global cursor object used across all game modes */
extern Cursor *g_cursor;

/** @brief Flag indicating if the game is running */
extern int running;

/**
 * @brief Main game loop function
 *
 * Executes the main game loop controlling updates and rendering.
 *
 * @return Exit status code (0 for success)
 */
int game_main_loop(void);

/**
 * @brief Handles keyboard input in menu mode
 *
 * Processes keyboard scancode input when in menu mode.
 *
 * @param scancode The keyboard scancode to process
 */
void menu_handle_input(uint8_t scancode);

#endif // GAME_H
