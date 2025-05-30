/**
 * @file gameover.h
 * @brief Header file for game over screen functionality
 *
 * This file provides functions to handle the game over state,
 * including initialization, input handling, and rendering.
 */

#ifndef GAMEOVER_H
#define GAMEOVER_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Initializes the game over screen
 * 
 * Sets up resources and initial state for the game over screen.
 */
void gameover_init(void);

/**
 * @brief Handles keyboard input on the game over screen
 * 
 * Processes keyboard input to navigate options after game over.
 * 
 * @param scancode The keyboard scancode to process
 */
void gameover_handle_input(uint8_t scancode);

/**
 * @brief Draws the game over screen
 * 
 * Renders the game over screen including score and options.
 */
void gameover_draw(void);

#endif // GAMEOVER_H
