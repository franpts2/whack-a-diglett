#include "sprite.h"
#include "../../controllers/video/video.h"
#include <stdio.h>

Sprite *sprite_create_from_xpm(xpm_map_t xpm_map) {
  Sprite *sprite = (Sprite *) malloc(sizeof(Sprite));
  if (sprite == NULL) {
    printf("Failed to allocate memory for sprite\n");
    return NULL;
  }

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

  // get current buffer to draw directly using the accessor function
  void *target_buffer = get_current_buffer();
  unsigned bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;

  // draw the sprite pixmap with optimized direct buffer access 
  // (faster than calling draw_pixel)
  for (uint16_t row = 0; row < sprite->height; row++) {
    // y position with bounds checking
    int y_pos = sprite->y + row;
    if (y_pos < 0 || y_pos >= m_info.YResolution)
      continue;

    for (uint16_t col = 0; col < sprite->width; col++) {
      // x position with bounds checking
      int x_pos = sprite->x + col;
      if (x_pos < 0 || x_pos >= m_info.XResolution)
        continue;

      uint32_t color_index = sprite->pixmap[row * sprite->width + col];

      // skip transparent pixels
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

void sprite_destroy(Sprite *sprite) {
  if (sprite != NULL) {
    if (sprite->pixmap != NULL) {
      free(sprite->pixmap);
    }
    free(sprite);
  }
}


int load_sprites(){
  background = sprite_create_from_xpm((xpm_map_t) background_xpm);
  title = sprite_create_from_xpm((xpm_map_t) title_xpm);
  instructions = sprite_create_from_xpm((xpm_map_t) instructions_xpm);
  dirt = sprite_create_from_xpm((xpm_map_t) dirt_xpm);
  cursor = sprite_create_from_xpm((xpm_map_t) cursor_xpm);
  diglett_appear0 = sprite_create_from_xpm((xpm_map_t) diglett_appear_0);
  diglett_appear1 = sprite_create_from_xpm((xpm_map_t) diglett_appear_1);
  diglett_appear2 = sprite_create_from_xpm((xpm_map_t) diglett_appear_2);
  diglett_appear3 = sprite_create_from_xpm((xpm_map_t) diglett_appear_3);
  diglett_appear4 = sprite_create_from_xpm((xpm_map_t) diglett_appear_4);
  diglett_appear5 = sprite_create_from_xpm((xpm_map_t) diglett_appear_5);
  diglett_appear6 = sprite_create_from_xpm((xpm_map_t) diglett_appear_6);
  diglett_appear7 = sprite_create_from_xpm((xpm_map_t) diglett_appear_7);
  diglett_appear8 = sprite_create_from_xpm((xpm_map_t) diglett_appear_8);
  diglett_appear9 = sprite_create_from_xpm((xpm_map_t) diglett_appear_9);
  diglett_boink0 = sprite_create_from_xpm((xpm_map_t) diglett_boink_0);
  diglett_boink1 = sprite_create_from_xpm((xpm_map_t) diglett_boink_2);
  diglett_boink3 = sprite_create_from_xpm((xpm_map_t) diglett_boink_3);
  diglett_boink4 = sprite_create_from_xpm((xpm_map_t) diglett_boink_4);
  diglett_boink5 = sprite_create_from_xpm((xpm_map_t) diglett_boink_5);
  diglett_boink6 = sprite_create_from_xpm((xpm_map_t) diglett_boink_6);
  diglett_boink7 = sprite_create_from_xpm((xpm_map_t) diglett_boink_7);
  return 0;
}
