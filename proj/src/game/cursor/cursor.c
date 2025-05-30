/**
 * @file cursor.c
 * @brief Implementation of mouse cursor functionality
 *
 * This file implements the functions to handle the mouse cursor.
 */

#include "cursor.h"
#include "../../controllers/kbdmouse/aux.h"
#include "../../controllers/video/video.h"
#include "../sprites/sprite.h"
#include "cursor_xpm.h"
#include <stdlib.h>

/** @brief Stores fractional x-movement for smooth cursor motion */
int mouse_x_remainder = 0;

/** @brief Stores fractional y-movement for smooth cursor motion */
int mouse_y_remainder = 0;

/** @brief Mouse acceleration factor */
float mouse_accel = 1.0;

/**
 * @brief Initializes a new cursor
 * 
 * Creates a cursor sprite from the cursor XPM image and positions it
 * at the center of the screen.
 * 
 * @return Pointer to the created cursor or NULL if initialization failed
 */
Cursor *cursor_init() {
  // Center of the screen (800x600 mode)
  int center_x = 400;
  int center_y = 300;
  return sprite_create_from_xpm((xpm_map_t) cursor_xpm, center_x, center_y);
}

/**
 * @brief Draws the cursor on the screen
 * 
 * Delegates to the sprite_draw function to render the cursor.
 * 
 * @param cursor Pointer to the cursor to draw
 * @return 0 on success, non-zero otherwise
 */
int cursor_draw(Cursor *cursor) {
  return sprite_draw(cursor);
}

/**
 * @brief Sets the cursor position
 * 
 * Updates the x and y coordinates of the cursor.
 * 
 * @param cursor Pointer to the cursor
 * @param x New x-coordinate
 * @param y New y-coordinate
 */
void cursor_set_position(Cursor *cursor, int x, int y) {
  if (cursor != NULL) {
    cursor->x = x;
    cursor->y = y;
  }
}

/**
 * @brief Frees resources used by the cursor
 * 
 * Delegates to the sprite_destroy function to free the cursor resources.
 * 
 * @param cursor Pointer to the cursor to destroy
 */
void cursor_destroy(Cursor *cursor) {
  sprite_destroy(cursor);
}

/**
 * @brief Updates cursor position based on mouse movement
 * 
 * Processes a mouse packet to update the cursor position with smooth movement,
 * applying dynamic acceleration and bounds checking.
 * 
 * @param cursor Pointer to the cursor
 * @param mouse_packet Pointer to the mouse packet with movement data
 */
void cursor_handle_mouse_packet(Cursor *cursor, struct packet *mouse_packet) {
  if (cursor == NULL || mouse_packet == NULL) {
    return;
  }
  
  float sensitivity = 2.2;
  
  // Fast path for zero movement - avoid calculations
  if (mouse_packet->delta_x == 0 && mouse_packet->delta_y == 0) {
    return;
  }
  
  // Dynamic acceleration based on movement speed
  int dx_abs = abs(mouse_packet->delta_x);
  int dy_abs = abs(mouse_packet->delta_y);
  int movement_intensity = (dx_abs > dy_abs) ? dx_abs + (dy_abs / 2) : dy_abs + (dx_abs / 2);
  
  // Responsive acceleration curve
  if (movement_intensity > 12) {
    mouse_accel = 2.2; // Hyper fast
  }
  else if (movement_intensity > 8) {
    mouse_accel = 1.8; // Fast
  }
  else if (movement_intensity > 4) {
    mouse_accel = 1.5; // Medium speed
  }
  else if (movement_intensity > 2) {
    mouse_accel = 1.2; // Slow
  }
  else {
    mouse_accel = 1.0; // Very slow movement
  }
  
  // Higher precision for smoother movement (5 extra bits)
  mouse_x_remainder += mouse_packet->delta_x * 32;
  mouse_y_remainder += mouse_packet->delta_y * 32;
  
  // Calculate movement
  int dx_applied = (mouse_x_remainder * sensitivity * mouse_accel) / 32;
  int dy_applied = (mouse_y_remainder * sensitivity * mouse_accel) / 32;
  
  // Remove applied movement from remainder with better precision
  mouse_x_remainder -= (dx_applied * 32) / (sensitivity * mouse_accel);
  mouse_y_remainder -= (dy_applied * 32) / (sensitivity * mouse_accel);
  
  // Apply movement
  int new_x = cursor->x + dx_applied;
  int new_y = cursor->y - dy_applied; // Y is inverted
  
  // Keep cursor inside screen boundaries (800x600)
  if (new_x < 0)
    new_x = 0;
  else if (new_x > 800 - cursor->width)
    new_x = 800 - cursor->width;
  
  if (new_y < 0)
    new_y = 0;
  else if (new_y > 600 - cursor->height)
    new_y = 600 - cursor->height;
  
  cursor_set_position(cursor, new_x, new_y);
}
