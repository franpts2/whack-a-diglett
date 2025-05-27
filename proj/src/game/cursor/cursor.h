#ifndef _CURSOR_H_
#define _CURSOR_H_

#include "../sprites/sprite.h"
#include "cursor_xpm.h"
#include <lcom/lcf.h>

// Cursor is now based on the Sprite structure
typedef Sprite Cursor;

/**
 * @brief Initialize a cursor object
 *
 * @return Cursor* Pointer to the created cursor object, NULL on failure
 */
Cursor *cursor_init();

/**
 * @brief Draw the cursor on the screen (now uses sprite_draw)
 *
 * @param cursor The cursor to draw
 * @return int 0 on success, non-zero on failure
 */
int cursor_draw(Cursor *cursor);

/**
 * @brief Set the cursor position
 *
 * @param cursor The cursor object
 * @param x X position
 * @param y Y position
 */
void cursor_set_position(Cursor *cursor, int x, int y);

/**
 * @brief Destroy the cursor object and free associated memory
 *
 * @param cursor The cursor to destroy
 */
void cursor_destroy(Cursor *cursor);

#endif /* _CURSOR_H_ */
