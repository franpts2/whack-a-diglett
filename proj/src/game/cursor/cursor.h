/**
 * @file cursor.h
 * @brief Header file for mouse cursor functionality
 *
 * This file provides the structure and functions to handle the mouse cursor.
 */

#ifndef _CURSOR_H_
#define _CURSOR_H_

#include "../sprites/sprite.h"
#include "cursor_xpm.h"
#include <lcom/lcf.h>

struct packet;

/**
 * @brief Cursor type based on Sprite
 * 
 * Uses the underlying Sprite structure to represent a mouse cursor.
 */
typedef Sprite Cursor;

/** @brief Stores fractional x-movement for smooth cursor motion */
extern int mouse_x_remainder;

/** @brief Stores fractional y-movement for smooth cursor motion */
extern int mouse_y_remainder;

/** @brief Mouse acceleration factor */
extern float mouse_accel;

/**
 * @brief Initializes a new cursor
 * 
 * Creates and initializes a cursor from the cursor XPM image.
 * 
 * @return Pointer to the created cursor or NULL if initialization failed
 */
Cursor *cursor_init();

/**
 * @brief Draws the cursor on the screen
 * 
 * @param cursor Pointer to the cursor to draw
 * @return 0 on success, non-zero otherwise
 */
int cursor_draw(Cursor *cursor);

/**
 * @brief Sets the cursor position
 * 
 * @param cursor Pointer to the cursor
 * @param x New x-coordinate
 * @param y New y-coordinate
 */
void cursor_set_position(Cursor *cursor, int x, int y);

/**
 * @brief Updates cursor position based on mouse movement
 * 
 * Processes a mouse packet to update the cursor position with smooth movement.
 * 
 * @param cursor Pointer to the cursor
 * @param mouse_packet Pointer to the mouse packet with movement data
 */
void cursor_handle_mouse_packet(Cursor *cursor, struct packet *mouse_packet);

/**
 * @brief Frees resources used by the cursor
 * 
 * @param cursor Pointer to the cursor to destroy
 */
void cursor_destroy(Cursor *cursor);

#endif /* _CURSOR_H_ */
