/**
 * @file pause.h
 * @brief Header file for game pause functionality
 *
 * This file provides functions to handle the game's pause state,
 * including initialization, input handling, and rendering.
 */

#ifndef _PAUSE_H_
#define _PAUSE_H_

#include "../game.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Initializes the pause screen
 * 
 * Sets up the initial state for the pause screen.
 */
void pause_init(void);

/**
 * @brief Handles keyboard input in pause mode
 * 
 * Processes keyboard scancodes to navigate pause options.
 * 
 * @param scancode The keyboard scancode to process
 */
void pause_handle_input(uint8_t scancode);

/**
 * @brief Draws the pause screen
 * 
 * Renders the pause screen with options.
 */
void draw_pause_screen(void);
void pause_resume_game(void);

#endif // _PAUSE_H_
