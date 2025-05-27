#include "sprite.h"
#include "../../controllers/video/video.h"
#include <stdio.h>

Sprite *sprite_create_from_xpm(xpm_map_t xpm_map, int x, int y) {
  Sprite *sprite = (Sprite *) malloc(sizeof(Sprite));
  if (sprite == NULL) {
    printf("Failed to allocate memory for sprite\n");
    return NULL;
  }

  // Initialize sprite position
  sprite->x = x;
  sprite->y = y;
  sprite->is_visible = true;

  // Load the XPM image
  sprite->pixmap = (uint32_t *) xpm_load(xpm_map, XPM_8_8_8_8, &sprite->img);
  if (sprite->pixmap == NULL) {
    printf("Failed to load XPM image\n");
    free(sprite);
    return NULL;
  }

  // Set width and height from the XPM image data
  sprite->width = sprite->img.width;
  sprite->height = sprite->img.height;

  return sprite;
}

int sprite_draw(Sprite *sprite) {
  if (sprite == NULL || sprite->pixmap == NULL || !sprite->is_visible)
    return 1;

  // Draw the sprite pixmap
  for (uint16_t row = 0; row < sprite->height; row++) {
    for (uint16_t col = 0; col < sprite->width; col++) {
      uint32_t color_index = sprite->pixmap[row * sprite->width + col];

      if (color_index == xpm_transparency_color(XPM_8_8_8_8))
        continue;

      // Skip drawing transparent pixels (assuming 0 is also transparent)
      if (color_index != 0) {
        draw_pixel(sprite->x + col, sprite->y + row, color_index);
      }
    }
  }

  return 0;
}

void sprite_destroy(Sprite *sprite) {
  if (sprite != NULL) {
    if (sprite->pixmap != NULL) {
      free(sprite->pixmap);
    }
    free(sprite);
  }
}
