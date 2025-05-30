/**
 * @file instructions.h
 * @brief Header file for game instructions functionality
 *
 * This file provides functions to handle the game's instructions screen,
 * including initialization, input handling, rendering, and cleanup.
 */

#ifndef _INSTRUCTIONS_H
#define _INSTRUCTIONS_H

#include "../game.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Initializes the instructions screen
 * 
 * Sets up the initial state for the instructions screen.
 */
void instructions_init(void);

/**
 * @brief Handles keyboard input in instructions mode
 * 
 * Processes keyboard scancodes to navigate the instructions screen.
 * 
 * @param scancode The keyboard scancode to process
 */
void instructions_handle_input(uint8_t scancode);

/**
 * @brief Draws the instructions screen
 * 
 * Renders the game instructions with text and graphics.
 */
void draw_instructions(void);

/**
 * @brief Frees resources used by the instructions screen
 * 
 * Cleans up any resources allocated for the instructions screen.
 */
void instructions_destroy(void);

#endif // _INSTRUCTIONS_H


