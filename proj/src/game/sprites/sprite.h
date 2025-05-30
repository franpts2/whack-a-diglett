/**
 * @file sprite.h
 * @brief Header file for sprite functionality
 *
 * This file provides structures and functions to handle static sprites.
 */

#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "../../controllers/video/video.h"
#include <lcom/lcf.h>
#include <stdlib.h>

/**
 * @brief Structure that represents a sprite
 * 
 * Contains all necessary information to handle a sprite,
 * including its position, image data, and visibility.
 */
typedef struct {
  int x;            /**< x position */
  int y;            /**< y position */
  uint32_t *pixmap; /**< pixmap containing the sprite's image */
  xpm_image_t img;  /**< XPM image information */
  bool is_visible;  /**< visibility flag */
  int width;        /**< width of the sprite */
  int height;       /**< height of the sprite */
} Sprite;

/**
 * @brief Creates a new sprite from an XPM image
 * 
 * @param xpm_map XPM map to create the sprite from
 * @param x Initial x-coordinate of the sprite
 * @param y Initial y-coordinate of the sprite
 * @return Pointer to the created Sprite or NULL if allocation failed
 */
Sprite *sprite_create_from_xpm(xpm_map_t xpm_map, int x, int y);

/**
 * @brief Draws the sprite onto the screen
 * 
 * @param sprite Pointer to the Sprite to draw
 * @return 0 on success, non-zero otherwise
 */
int sprite_draw(Sprite *sprite);

/**
 * @brief Frees memory used by a sprite
 * 
 * @param sprite Pointer to the Sprite to destroy
 */
void sprite_destroy(Sprite *sprite);

#endif /* _SPRITE_H_ */
