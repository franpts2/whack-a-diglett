/**
 * @file background.h
 * @brief Header file for game background management
 *
 * This file provides functions to initialize, draw, and destroy
 * the game background.
 */

#ifndef _BACKGROUND_H_
#define _BACKGROUND_H_

/**
 * @brief Initializes the background
 * 
 * Creates and initializes the sprites for the game background.
 * 
 * @return 0 on success, non-zero otherwise
 */
int background_init();

/**
 * @brief Draws the background to the screen
 * 
 * Renders the background sprites to the current buffer.
 * 
 * @return 0 on success, non-zero otherwise
 */
int background_draw();

/**
 * @brief Frees resources used by the background
 * 
 * Deallocates memory used by the background sprites.
 */
void background_destroy();

#endif // _BACKGROUND_H_
