#include "title.h"
#include "../controllers/video/video.h"
#include "sprites/pixelart/title_xpm.h"
#include "sprites/sprite.h"

int title_init() {
  if (title != NULL) {
    sprite_destroy(title);
  }


  if (title == NULL) {
    printf("Failed to create title sprite.\n");
    return 1;
  }

  printf("Title sprite created successfully with dimensions: %d x %d\n",
         title->width, title->height);

  int x_pos = (800 - title->width) / 2;
  if (x_pos < 0)
    x_pos = 0; // ensure it's not off-screen
  title->x = x_pos;

  title->y = 10;

  return 0;
}

int title_draw() {
  if (title == NULL) {
    printf("Title sprite is not initialized\n");
    return 1;
  }

  if (sprite_draw(title, title->x, title->y, (uint8_t *)get_current_buffer()) != 0) {
    printf("Failed to draw title sprite\n");
    return 1;
  }

  return 0;
}

void title_destroy() {
  if (title != NULL) {
    sprite_destroy(title);
    title = NULL;
  }
}
