/**
 * @file playing_kbd.h
 * @brief Header file for keyboard-specific gameplay functions
 *
 * This file provides functions for keyboard-specific gameplay,
 * including input handling and keyboard-specific rendering.
 */

#ifndef _PLAYING_KBD_H
#define _PLAYING_KBD_H

#include "playing.h"

/**
 * @brief Initializes the keyboard gameplay mode
 * 
 * Sets up the game state specifically for keyboard controls.
 */
void playing_kbd_init(void);

/**
 * @brief Updates the keyboard gameplay mode state
 * 
 * Handles game logic updates for keyboard-based gameplay.
 */
void playing_kbd_update(void);

/**
 * @brief Processes keyboard input during gameplay
 * 
 * Handles keyboard scancodes to interact with the game elements.
 * 
 * @param scancode The keyboard scancode to process
 */
void playing_handle_input(uint8_t scancode);

/**
 * @brief Draws a diglett with keyboard-specific visuals
 * 
 * Renders a specific diglett with keyboard mode appearance.
 * 
 * @param index The index of the diglett to draw
 */
void draw_kbd_diglett(int index);

/**
 * @brief Draws the keyboard key label for a diglett
 * 
 * Displays the associated keyboard key for each diglett.
 * 
 * @param index The index of the diglett for which to draw the label
 */
void draw_kbd_diglett_label(int index);

/** @brief Array of keyboard keys assigned to each diglett */
extern const uint8_t diglett_keys[NUM_DIGLETTS];

#endif // _PLAYING_KBD_H
