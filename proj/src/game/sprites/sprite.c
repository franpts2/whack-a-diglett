#include "sprite.h"
#include "../../controllers/video/video.h"

/**
 * @brief Draw a sprite on the screen
 *
 * @param sprite The sprite to draw
 * @return int 0 on success, non-zero on failure
 */
int sprite_draw(Sprite *sprite) {
  if (sprite == NULL || sprite->pixmap == NULL || !sprite->is_visible)
    return 1;

  // Draw the sprite pixmap
  for (uint16_t row = 0; row < sprite->img.height; row++) {
    for (uint16_t col = 0; col < sprite->img.width; col++) {
      uint32_t color_index = sprite->pixmap[row * sprite->img.width + col];

      if (color_index == xpm_transparency_color(XPM_8_8_8_8)) {
        // Skip drawing transparent pixels
        continue;
      }

      // Skip drawing transparent pixels (assuming 0 is also transparent)
      if (color_index != 0) {
        vg_draw_rectangle(sprite->x + col, sprite->y + row, 1, 1, color_index);
      }
    }
  }

  return 0;
}