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
void gameover_update_selection(void);
void gameover_select_option(void);

void gameover_handle_mouse(int x, int y, bool left_button_pressed);

void draw_gameover_bg_and_buttons(void);
void draw_gameover_selection(void);

extern int gameover_selected;

#endif // GAMEOVER_H
