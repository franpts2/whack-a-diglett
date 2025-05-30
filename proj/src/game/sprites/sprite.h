#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "../../controllers/video/video.h"
#include <lcom/lcf.h>
#include <stdlib.h>
#include "pixelart/background_xpm.h"
#include "pixelart/dirt_xpm.h"
#include "pixelart/instructions_xpm.h"
#include "pixelart/title_xpm.h"
#include "animations/diglett_appear_xpm.h"
#include "animations/diglett_boink_xpm.h"
#include "../cursor/cursor_xpm.h"
#include "../../controllers/video/video.h"

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
Sprite *diglett_appear0;
Sprite *diglett_appear1;
Sprite *diglett_appear2;
Sprite *diglett_appear3;
Sprite *diglett_appear4;
Sprite *diglett_appear5;
Sprite *diglett_appear6;
Sprite *diglett_appear7;
Sprite *diglett_appear8;
Sprite *diglett_appear9;
Sprite *diglett_boink0;
Sprite *diglett_boink1;
Sprite *diglett_boink2;
Sprite *diglett_boink3;
Sprite *diglett_boink4;
Sprite *diglett_boink5;
Sprite *diglett_boink6;
Sprite *diglett_boink7;

Sprite *sprite_create_from_xpm(xpm_map_t xpm_map, int x, int y);

int sprite_draw(Sprite *sprite, uint16_t x, uint16_t y, uint8_t *buffer);

void sprite_destroy(Sprite *sprite);

#endif /* _SPRITE_H_ */
