/**
 * @file animated_sprite.c
 * @brief Implementation of animated sprite functionality
 *
 * This file contains functions to create, update, draw, and destroy animated sprites.
 */

#include "animated_sprite.h"
#include <stdlib.h>

/**
 * @brief Creates a new animated sprite
 * 
 * Allocates memory for a new AnimatedSprite and initializes all its properties.
 * Creates all frames from XPM maps and positions the sprite at the specified coordinates.
 * 
 * @param xpm_maps Array of XPM maps for each animation frame
 * @param num_frames Number of frames in the animation
 * @param x Initial x-coordinate of the sprite
 * @param y Initial y-coordinate of the sprite
 * @param frame_delay Number of updates to wait before advancing to the next frame
 * @return Pointer to the created AnimatedSprite or NULL if allocation failed
 */
AnimatedSprite *animated_sprite_create(xpm_map_t *xpm_maps, int num_frames, int x, int y, int frame_delay) {

  AnimatedSprite *anim = malloc(sizeof(AnimatedSprite));
  if (!anim) return NULL;
  anim->frames = malloc(sizeof(Sprite*) * num_frames);
  if (!anim->frames) { free(anim); return NULL; }
  for (int i = 0; i < num_frames; i++) {
    anim->frames[i] = sprite_create_from_xpm(xpm_maps[i], x, y);
    if (!anim->frames[i]) {
      for (int j = 0; j < i; j++) sprite_destroy(anim->frames[j]);
      free(anim->frames); free(anim); return NULL;
    }
  }
  anim->frame_count = num_frames;
  anim->current_frame = 0;
  anim->x = x;
  anim->y = y;
  anim->frame_delay = frame_delay;
  anim->frame_timer = 0;
  anim->is_visible = true;
  return anim;
}

/**
 * @brief Updates the animated sprite's state
 * 
 * Increments the frame timer and advances to the next frame when the frame delay is reached.
 * The animation loops back to the first frame after the last frame.
 * 
 * @param anim Pointer to the AnimatedSprite to update
 */
void animated_sprite_update(AnimatedSprite *anim) {
  if (!anim || !anim->is_visible) return;
  anim->frame_timer++;
  if (anim->frame_timer >= anim->frame_delay) {
    anim->current_frame = (anim->current_frame + 1) % anim->frame_count;
    anim->frame_timer = 0;
  }
}

/**
 * @brief Draws the current frame of the animated sprite
 * 
 * Updates the position of the current frame and draws it to the screen.
 * 
 * @param anim Pointer to the AnimatedSprite to draw
 * @return 0 on success, non-zero otherwise
 */
int animated_sprite_draw(AnimatedSprite *anim) {
  if (!anim || !anim->is_visible) return 1;
  Sprite *frame = anim->frames[anim->current_frame];
  frame->x = anim->x;
  frame->y = anim->y;
  frame->is_visible = true;
  return sprite_draw(frame);
}

/**
 * @brief Frees all memory used by an animated sprite
 * 
 * Destroys all frame sprites and frees the memory allocated for the AnimatedSprite.
 * 
 * @param anim Pointer to the AnimatedSprite to destroy
 */
void animated_sprite_destroy(AnimatedSprite *anim) {
  if (!anim) return;
  for (int i = 0; i < anim->frame_count; i++) {
    sprite_destroy(anim->frames[i]);
  }
  free(anim->frames);
  free(anim);
}
