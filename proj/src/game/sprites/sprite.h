#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <lcom/lcf.h>
#include <stdlib.h>

typedef struct {
  int x;            // x position
  int y;            // y position
  uint32_t *pixmap; // pixmap containing the sprite's image
  xpm_image_t img;  // XPM image information
  bool is_visible;  // visibility flag
  int width;        // width of the sprite
  int height;       // height of the sprite
} Sprite;

Sprite *sprite_create_from_xpm(xpm_map_t xpm_map, int x, int y);

int sprite_draw(Sprite *sprite);

void sprite_destroy(Sprite *sprite);

#endif /* _SPRITE_H_ */
