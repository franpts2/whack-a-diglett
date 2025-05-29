#include "background.h"
#include "../controllers/video/video.h"
#include "sprites/pixelart/background_xpm.h"
#include "sprites/sprite.h"

// Global background sprite
static Sprite *background_sprite = NULL;

/**
 * @brief Initialize the background
 *
 * @return int 0 if successful, 1 otherwise
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
 * @brief Draw the background to the current buffer
 *
 * @return int 0 if successful, 1 otherwise
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
 * @brief Clean up the background resources
 */
void background_destroy() {
  if (background_sprite != NULL) {
    sprite_destroy(background_sprite);
    background_sprite = NULL;
  }
}
