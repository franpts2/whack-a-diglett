#ifndef _ANIMATED_SPRITE_H_
#define _ANIMATED_SPRITE_H_

#include "sprite.h"

typedef struct {
  Sprite **frames;      
  int frame_count;      
  int current_frame;    
  int x, y;             
  int frame_delay;      
  int frame_timer;      
  bool is_visible;      
} AnimatedSprite;

AnimatedSprite *animated_sprite_create(xpm_map_t *xpm_maps, int num_frames, int x, int y, int frame_delay);

void animated_sprite_update(AnimatedSprite *anim_sprite);

int animated_sprite_draw(AnimatedSprite *anim_sprite);

void animated_sprite_destroy(AnimatedSprite *anim_sprite);

#endif 
