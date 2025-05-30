#include "animated_sprite.h"
#include <stdlib.h>

AnimatedSprite *animated_sprite_create(xpm_map_t *xpm_maps, int num_frames, int x, int y, int frame_delay) {

  AnimatedSprite *anim = malloc(sizeof(AnimatedSprite));
  if (!anim) return NULL;
  anim->frames = malloc(sizeof(Sprite*) * num_frames);
  if (!anim->frames) { free(anim); return NULL; }
  for (int i = 0; i < num_frames; i++) {
    anim->frames[i] = sprite_create_from_xpm(xpm_maps[i]);
    if (!anim->frames[i]) {
      for (int j = 0; j < i; j++) sprite_destroy(anim->frames[j]);
      free(anim->frames); free(anim); return NULL;
    }
  }
  anim->frame_count = num_frames;
  anim->current_frame = 0;
  anim->x = x;
  anim->y = y;
  anim->frame_delay = frame_delay;
  anim->frame_timer = 0;
  anim->is_visible = true;
  return anim;
}

void animated_sprite_update(AnimatedSprite *anim) {
  if (!anim || !anim->is_visible) return;
  anim->frame_timer++;
  if (anim->frame_timer >= anim->frame_delay) {
    anim->current_frame = (anim->current_frame + 1) % anim->frame_count;
    anim->frame_timer = 0;
  }
}

int animated_sprite_draw(AnimatedSprite *anim) {
  if (!anim || !anim->is_visible) return 1;
  Sprite *frame = anim->frames[anim->current_frame];
  frame->x = anim->x;
  frame->y = anim->y;
  frame->is_visible = true;
  return sprite_draw(frame, frame->x, frame->y, NULL);
}

void animated_sprite_destroy(AnimatedSprite *anim) {
  if (!anim) return;
  for (int i = 0; i < anim->frame_count; i++) {
    sprite_destroy(anim->frames[i]);
  }
  free(anim->frames);
  free(anim);
}
