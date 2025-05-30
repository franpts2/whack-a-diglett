/**
 * @file sprite.c
 * @brief Implementation of sprite functionality
 *
 * This file implements functions to create, draw, and destroy sprites.
 */

#include "sprite.h"
#include "../../controllers/video/video.h"
#include <stdio.h>

/**
 * @brief Creates a new sprite from an XPM image
 * 
 * Allocates memory for a Sprite structure and initializes its properties.
 * Loads the XPM image into a pixmap and stores size information.
 * 
 * @param xpm_map XPM map to create the sprite from
 * @param x Initial x-coordinate of the sprite
 * @param y Initial y-coordinate of the sprite
 * @return Pointer to the created Sprite or NULL if allocation failed
 */
Sprite *sprite_create_from_xpm(xpm_map_t xpm_map, int x, int y) {
  Sprite *sprite = (Sprite *) malloc(sizeof(Sprite));
  if (sprite == NULL) {
    printf("Failed to allocate memory for sprite\n");
    return NULL;
  }

  sprite->x = x;
  sprite->y = y;
  sprite->is_visible = true;

  sprite->pixmap = (uint32_t *) xpm_load(xpm_map, XPM_8_8_8_8, &sprite->img);
  if (sprite->pixmap == NULL) {
    printf("Failed to load XPM image\n");
    free(sprite);
    return NULL;
  }

  sprite->width = sprite->img.width;
  sprite->height = sprite->img.height;
  return sprite;
}

/**
 * @brief Draws the sprite onto the screen
 * 
 * Copies the sprite's pixmap data to the current video buffer,
 * skipping transparent pixels and performing bounds checking.
 * 
 * @param sprite Pointer to the Sprite to draw
 * @return 0 on success, non-zero otherwise
 */
int sprite_draw(Sprite *sprite) {
  if (sprite == NULL || sprite->pixmap == NULL || !sprite->is_visible)
    return 1;

  void *target_buffer = get_current_buffer();
  unsigned bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;

  for (uint16_t row = 0; row < sprite->height; row++) {
    int y_pos = sprite->y + row;
    if (y_pos < 0 || y_pos >= m_info.YResolution)
      continue;

    for (uint16_t col = 0; col < sprite->width; col++) {
      int x_pos = sprite->x + col;
      if (x_pos < 0 || x_pos >= m_info.XResolution)
        continue;

      uint32_t color_index = sprite->pixmap[row * sprite->width + col];
      
      if (color_index == xpm_transparency_color(XPM_8_8_8_8) || color_index == 0)
        continue;

      unsigned int pixel_pos = (y_pos * m_info.XResolution + x_pos) * bytes_per_pixel;
      for (unsigned i = 0; i < bytes_per_pixel; i++) {
        *((uint8_t *) target_buffer + pixel_pos + i) = (color_index >> (i * 8)) & 0xFF;
      }
    }
  }
  return 0;
}

/**
 * @brief Frees memory used by a sprite
 * 
 * Deallocates the pixmap and the Sprite structure.
 * 
 * @param sprite Pointer to the Sprite to destroy
 */
void sprite_destroy(Sprite *sprite) {
  if (sprite != NULL) {
    if (sprite->pixmap != NULL) {
      free(sprite->pixmap);
    }
    free(sprite);
  }
}
