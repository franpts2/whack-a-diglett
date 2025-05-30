/**
 * @file playing.h
 * @brief Header file for main gameplay functionality
 *
 * This file provides definitions and functions for the main gameplay mode,
 * including the diglett game elements, scoring, and game timing.
 */

#ifndef _PLAYING_H
#define _PLAYING_H

#include "../../game.h"
#include <stdbool.h>
#include <stdint.h>

/** @brief Background color in RGB */
#define BACKGROUND_COLOR 0x8DC740

/** @brief Diglett color in RGB */
#define DIGLETT_COLOR 0x885500

/** @brief Minimum frames a diglett stays visible (1 second at 60Hz) */
#define MIN_DIGLETT_SHOW_TIME 60  

/** @brief Maximum frames a diglett stays visible (3 seconds at 60Hz) */
#define MAX_DIGLETT_SHOW_TIME 180 

/** @brief Minimum frames a diglett stays hidden (1 second at 60Hz) */
#define MIN_DIGLETT_HIDE_TIME 60  

/** @brief Maximum frames a diglett stays hidden (2 seconds at 60Hz) */
#define MAX_DIGLETT_HIDE_TIME 120 

/** @brief Maximum number of digletts visible at once */
#define MAX_VISIBLE_DIGLETTS 5    

/** @brief Total game time in seconds */
#define TIMER_BAR_TOTAL_SECONDS 60

/** @brief Width of the timer bar in pixels */
#define TIMER_BAR_WIDTH 160 

/** @brief Height of the timer bar in pixels */
#define TIMER_BAR_HEIGHT 20

/** @brief X-coordinate of the timer bar */
#define TIMER_BAR_X 50 
#define TIMER_BAR_Y 15

/**
 * @brief Structure representing a diglett game element
 * 
 * Contains all properties of a diglett including position, visibility,
 * timing, and animation states.
 */
typedef struct {
  int x;            /**< x position of the rectangle */
  int y;            /**< y position of the rectangle */
  int width;        /**< width of the rectangle */
  int height;       /**< height of the rectangle */
  uint8_t key;      /**< keyboard scancode for this diglett */
  bool visible;     /**< whether the diglett is currently visible */
  int timer;        /**< countdown timer for state changes */
  bool active;      /**< whether the diglett is in play (for game progression) */
  bool boinking;    /**< whether the diglett is in boink animation */
  int boink_timer;  /**< timer for boink animation */
  bool timing_out;  /**< whether the diglett is timing out */
  int timeout_timer; /**< timer for timeout animation */
} Diglett;

/** @brief Total number of digletts in the game */
#define NUM_DIGLETTS 9

/** @brief Array of all digletts in the game */
extern Diglett digletts[NUM_DIGLETTS];

/** @brief Current number of visible digletts */
extern int visible_diglett_count;

/** @brief Player's current score */
extern int player_points;

/**
 * @brief Initializes the playing game mode
 * 
 * Sets up the initial game state including digletts positioning and timing.
 * 
 * @param is_kbd True if keyboard mode, false if mouse mode
 */
void playing_init(bool is_kbd);

/**
 * @brief Updates the game state each frame
 * 
 * Handles diglett timing, animations, and state transitions.
 * 
 * @param is_kbd True if keyboard mode, false if mouse mode
 */
void playing_update(bool is_kbd);

/**
 * @brief Draws a specific diglett
 * 
 * Renders a diglett at its position with the appropriate animation frame.
 * 
 * @param index The index of the diglett to draw
 * @param is_kbd True if keyboard mode, false if mouse mode
 */
void draw_diglett(int index, bool is_kbd);

/**
 * @brief Draws the player's score
 * 
 * Displays the current player score on the screen.
 */
void draw_points_counter(void);

/**
 * @brief Handles a player's attempt to hit a diglett
 * 
 * Processes the player's action and updates score if successful.
 * 
 * @param index The index of the diglett being hit
 * @return True if the hit was successful, false otherwise
 */
bool whack_diglett(int index);

/**
 * @brief Handles common input logic for both input modes
 * 
 * Processes player input that is shared between keyboard and mouse modes.
 * 
 * @param index The index of the diglett being interacted with
 * @param is_kbd True if keyboard mode, false if mouse mode
 */
void playing_handle_common_input(int index, bool is_kbd);

/**
 * @brief Generates a random timer value
 * 
 * Creates a random value between min and max for diglett timing.
 * 
 * @param min Minimum value for timer
 * @param max Maximum value for timer
 * @return Random timer value
 */
int get_random_timer(int min, int max);

/**
 * @brief Sets the playing mode
 * 
 * Configures the game for either keyboard or mouse input.
 * 
 * @param is_keyboard_mode True for keyboard mode, false for mouse mode
 */
void set_playing_mode(bool is_keyboard_mode);

/**
 * @brief Draws the game timer bar
 * 
 * Displays the remaining game time as a visual bar.
 */
void draw_timer_bar(void);

#endif // _PLAYING_H
