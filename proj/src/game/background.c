/**
 * @file background.c
 * @brief Implementation of game background functionality
 *
 * This file implements functions to initialize, draw, and destroy
 * the game background sprite.
 */

#include "background.h"
#include "../controllers/video/video.h"
#include "sprites/pixelart/background_xpm.h"
#include "sprites/sprite.h"

/** @brief Global background sprite pointer */
static Sprite *background_sprite = NULL;

/**
 * @brief Initializes the background
 * 
 * Creates and initializes the sprite for the game background.
 * If the sprite already exists, it will be destroyed and recreated.
 * 
 * @return 0 on success, non-zero otherwise
 */
int background_init() {
  if (background_sprite != NULL) {
    sprite_destroy(background_sprite);
  }
  // Create the background sprite at position (0,0)
  background_sprite = sprite_create_from_xpm((xpm_map_t) background_xpm, 0, 0);
  if (background_sprite == NULL) {
    printf("Failed to create background sprite\n");
    return 1;
  }
  return 0;
}

/**
 * @brief Draws the background to the screen
 * 
 * Renders the background sprite to the current buffer.
 * Initializes the background if it hasn't been created yet.
 * 
 * @return 0 on success, non-zero otherwise
 */
int background_draw() {
  if (background_sprite == NULL) {
    if (background_init() != 0) {
      return 1;
    }
  }
  return sprite_draw(background_sprite);
}

/**
 * @brief Frees resources used by the background
 * 
 * Deallocates memory used by the background sprite and
 * sets the pointer to NULL to prevent use-after-free.
 */
void background_destroy() {
  if (background_sprite != NULL) {
    sprite_destroy(background_sprite);
    background_sprite = NULL;
  }
}
