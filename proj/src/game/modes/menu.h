/**
 * @file menu.h
 * @brief Header file for game menu functionality
 *
 * This file provides functions to handle the game's main menu,
 * including selection, input handling, and rendering.
 */

#ifndef MENU_H
#define MENU_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Initializes the menu
 * 
 * Sets up initial menu state and resources.
 */
void menu_init(void);

/**
 * @brief Handles keyboard input for the menu
 * 
 * Processes keyboard scancodes to navigate the menu.
 * 
 * @param scancode The keyboard scancode to process
 */
void menu_handle_input(uint8_t scancode);

/**
 * @brief Updates the currently selected menu option
 * 
 * Updates the selection based on keyboard navigation.
 */
void menu_update_selection(void);

/**
 * @brief Activates the currently selected menu option
 * 
 * Performs the action associated with the selected menu item.
 */
void menu_select_option(void);

/**
 * @brief Handles mouse input for the menu
 * 
 * Processes mouse position and clicks to navigate and select menu options.
 * 
 * @param x Mouse x-coordinate
 * @param y Mouse y-coordinate
 * @param left_button_pressed Whether the left mouse button was clicked
 */
void menu_handle_mouse(int x, int y, bool left_button_pressed);

/**
 * @brief Draws the menu background and buttons
 * 
 * Renders the static elements of the menu screen.
 */
void draw_menu_bg_and_buttons(void);

/**
 * @brief Draws the menu selection highlight
 * 
 * Renders the visual indicator for the currently selected option.
 */
void draw_menu_selection(void);

/** @brief Currently selected menu option index */
extern int selected;

#endif // MENU_H
