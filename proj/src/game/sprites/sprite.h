#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <lcom/lcf.h>

typedef struct {
  int x;              // x position
  int y;              // y position
  uint32_t *pixmap;   // pixmap containing the sprite's image
  xpm_image_t img;    // XPM image information
  bool is_visible;    // visibility flag
} Sprite;

/**
 * @brief Draw a sprite on the screen
 * 
 * @param sprite The sprite to draw
 * @return int 0 on success, non-zero on failure
 */
int sprite_draw(Sprite *sprite);

#endif /* _SPRITE_H_ */