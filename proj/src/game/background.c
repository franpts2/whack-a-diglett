#include "background.h"
#include "../controllers/video/video.h"
#include "sprites/pixelart/background_xpm.h"
#include "sprites/sprite.h"

// Global background sprite


int background_init() {
  if (background != NULL) {
    sprite_destroy(background);
  }

  if (background == NULL) {
    printf("Failed to create background sprite\n");
    return 1;
  }

  return 0;
}

int background_draw() {
  if (background == NULL) {
    if (background_init() != 0) {
      return 1;
    }
  }

  return sprite_draw(background, 0, 0, NULL);
}

void background_destroy() {
  if (background != NULL) {
    sprite_destroy(background);
    background = NULL;
  }
}
