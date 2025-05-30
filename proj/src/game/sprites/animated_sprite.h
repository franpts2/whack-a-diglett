/**
 * @file animated_sprite.h
 * @brief Header file for animated sprite functionality
 *
 * This file provides structures and functions to handle animated sprites.
 */

#ifndef _ANIMATED_SPRITE_H_
#define _ANIMATED_SPRITE_H_

#include "sprite.h"

/**
 * @brief Structure that represents an animated sprite
 * 
 * Contains all necessary information to handle an animated sprite,
 * including its frames, position, timing, and visibility.
 */
typedef struct {
  Sprite **frames;      /**< Array of sprite frames */
  int frame_count;      /**< Total number of frames in the animation */
  int current_frame;    /**< Current frame being displayed */
  int x, y;             /**< Coordinates of the sprite */
  int frame_delay;      /**< Number of updates to wait before advancing to the next frame */
  int frame_timer;      /**< Counter for tracking time between frame changes */
  bool is_visible;      /**< Whether the sprite should be drawn */
} AnimatedSprite;

/**
 * @brief Creates a new animated sprite
 * 
 * @param xpm_maps Array of XPM maps for each animation frame
 * @param num_frames Number of frames in the animation
 * @param x Initial x-coordinate of the sprite
 * @param y Initial y-coordinate of the sprite
 * @param frame_delay Number of updates to wait before advancing to the next frame
 * @return Pointer to the created AnimatedSprite or NULL if allocation failed
 */
AnimatedSprite *animated_sprite_create(xpm_map_t *xpm_maps, int num_frames, int x, int y, int frame_delay);

/**
 * @brief Updates the animated sprite's state
 * 
 * @param anim_sprite Pointer to the AnimatedSprite to update
 */
void animated_sprite_update(AnimatedSprite *anim_sprite);

/**
 * @brief Draws the current frame of the animated sprite
 * 
 * @param anim_sprite Pointer to the AnimatedSprite to draw
 * @return 0 on success, non-zero otherwise
 */
int animated_sprite_draw(AnimatedSprite *anim_sprite);

/**
 * @brief Frees all memory used by an animated sprite
 * 
 * @param anim_sprite Pointer to the AnimatedSprite to destroy
 */
void animated_sprite_destroy(AnimatedSprite *anim_sprite);

#endif
