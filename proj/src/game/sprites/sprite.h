#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "../../controllers/video/video.h"
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

Sprite *background;
Sprite *title;
Sprite *instructions;
Sprite *dirt;
Sprite *cursor;
Sprite *diglett_appear_0;
Sprite *diglett_appear_1;
Sprite *diglett_appear_2;
Sprite *diglett_appear_3;
Sprite *diglett_appear_4;
Sprite *diglett_appear_5;
Sprite *diglett_appear_6;
Sprite *diglett_appear_7;
Sprite *diglett_appear_8;
Sprite *diglett_appear_9;
Sprite *diglett_boink_0;
Sprite *diglett_boink_1;
Sprite *diglett_boink_2;
Sprite *diglett_boink_3;
Sprite *diglett_boink_4;
Sprite *diglett_boink_5;
Sprite *diglett_boink_6;
Sprite *diglett_boink_7;

Sprite *sprite_create_from_xpm(xpm_map_t xpm_map, int x, int y);

int sprite_draw(Sprite *sprite);

void sprite_destroy(Sprite *sprite);

#endif /* _SPRITE_H_ */
