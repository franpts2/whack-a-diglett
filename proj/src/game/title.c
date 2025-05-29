#include "title.h"
#include "../controllers/video/video.h"
#include "sprites/pixelart/title_xpm.h"
#include "sprites/sprite.h"

Sprite *title_sprite = NULL;

int title_init() {
  if (title_sprite != NULL) {
    sprite_destroy(title_sprite);
  }

  title_sprite = sprite_create_from_xpm((xpm_map_t) title_xpm, 0, 0);

  if (title_sprite == NULL) {
    printf("Failed to create title sprite.\n");
    return 1;
  }

  printf("Title sprite created successfully with dimensions: %d x %d\n",
         title_sprite->width, title_sprite->height);

  int x_pos = (800 - title_sprite->width) / 2;
  if (x_pos < 0)
    x_pos = 0; // ensure it's not off-screen
  title_sprite->x = x_pos;

  title_sprite->y = 25;

  return 0;
}

int title_draw() {
  if (title_sprite == NULL) {
    printf("Title sprite is not initialized\n");
    return 1;
  }

  if (sprite_draw(title_sprite) != 0) {
    printf("Failed to draw title sprite\n");
    return 1;
  }

  return 0;
}

void title_destroy() {
  if (title_sprite != NULL) {
    sprite_destroy(title_sprite);
    title_sprite = NULL;
  }
}
