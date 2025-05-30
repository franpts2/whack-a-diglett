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

int sprite_draw(Sprite *sprite, uint16_t x, uint16_t y, uint8_t *buffer) {

  int height = sprite->height;
  int width = sprite->width;

  uint32_t color;

  for (int row = 0; row < height; row++){
    for (int col = 0; col < width; col++){
      color = sprite->pixmap[row * sprite->width + col];
      if (color == xpm_transparency_color(XPM_8_8_8_8)) continue;
      if (draw_pixel(x + col, y + row, color, buffer) != 0) return 1;
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

void free_sprites(){
  sprite_destroy(background);
  sprite_destroy(title);
  sprite_destroy(instructions);
  sprite_destroy(dirt);
  sprite_destroy(cursor);
  sprite_destroy(diglett_appear0);
  sprite_destroy(diglett_appear1);
  sprite_destroy(diglett_appear2);
  sprite_destroy(diglett_appear3);
  sprite_destroy(diglett_appear4);
  sprite_destroy(diglett_appear5);
  sprite_destroy(diglett_appear6);
  sprite_destroy(diglett_appear7);
  sprite_destroy(diglett_appear8);
  sprite_destroy(diglett_appear9);
  sprite_destroy(diglett_boink0);
  sprite_destroy(diglett_boink1);
  sprite_destroy(diglett_boink2);
  sprite_destroy(diglett_boink3);
  sprite_destroy(diglett_boink4);
  sprite_destroy(diglett_boink5);
  sprite_destroy(diglett_boink6);
  sprite_destroy(diglett_boink7);
}