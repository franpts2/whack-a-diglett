#include "playing.h"
#include "../../../controllers/video/video.h"
#include "../../../fonts/testfont.h"
#include <game/background.h>
#include <game/sprites/animated_sprite.h>
#include <game/sprites/animations/diglett_appear_xpm.h>
#include <game/sprites/pixelart/dirt_xpm.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Forward declarations
void draw_background(void);
void playing_destroy(void);

// common game variables
Diglett digletts[NUM_DIGLETTS];
int visible_diglett_count = 0;
int player_points = 0;

extern xpm_map_t diglett_appear_frames[];
AnimatedSprite *diglett_sprites[NUM_DIGLETTS] = {0};

static bool using_keyboard_mode = true;

int get_random_timer(int min, int max) {
  return min + (rand() % (max - min + 1));
}

void draw_points_counter() {
  // caclcular posição pro counter de pontos nao sair da tela
  int counter_width = 200;
  int counter_x = 800 - counter_width - 10;

  // width needed for "Points: " (7 chars * 8 pixels * 2 scale)
  int points_label_width = 8 * 8 * 2;

  // position for the points number
  int number_area_x = counter_x + 10 + points_label_width;

  // don't exceed screen boundaries
  int max_number_width = counter_width - points_label_width - 20;

  // clear the number area
  vg_draw_rectangle(number_area_x, 10, max_number_width, 30, BACKGROUND_COLOR);

  char points_str[20] = {0};

  int safe_points = player_points;
  if (safe_points < 0)
    safe_points = 0;
  if (safe_points > 999)
    safe_points = 999;

  sprintf(points_str, "%d", safe_points);

  draw_text_scaled(points_str, number_area_x, 10, 0xFFFFFF, 2);
}

void playing_init(bool is_kbd) {
  // seed the random number generator
  srand(time(NULL));

  visible_diglett_count = 0;
  player_points = 0;

  extern GameMode prev_mode;
  extern Cursor *g_cursor;

  // hide cursor in kbd playing mode by moving it off-screen (it won't be rendered anyway)
  if (g_cursor != NULL) {
    cursor_set_position(g_cursor, -100, -100);
  }

  prev_mode = MODE_PLAYING; // mark transition as complete

  // reset the screen completely - crucial to remove menu artifacts
  extern void *back_buffer;
  extern void *static_buffer;
  extern void *middle_buffer;
  unsigned int bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
  unsigned int buffer_size = m_info.XResolution * m_info.YResolution * bytes_per_pixel;

  // clear all buffers
  memset(back_buffer, 0, buffer_size);
  memset(middle_buffer, 0, buffer_size);
  memset(static_buffer, 0, buffer_size);

  // set to draw to static buffer for the game background - IMPORTANT
  set_drawing_to_static();
  background_init(); // Initialize the background

  // titulo centrado
  int title_scale = 3;
  const char *title = "WHACK'A DIGLETT";
  int title_width = strlen(title) * 8 * title_scale;
  int title_x = (800 - title_width) / 2;
  draw_text_scaled(title, title_x, 50, 0xFFFFFF, title_scale);

  int rect_width = 60;
  int rect_height = 80;
  int spacing = 60; // space between digletts

  int grid_width = 3 * rect_width + 2 * spacing;

  // calculate starting position to center the grid
  int start_x = (800 - grid_width) / 2;
  int start_y = 150;

  // 3x3 grid - draw holes to static buffer
  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
      int index = row * 3 + col;
      int x = start_x + col * (rect_width + spacing);
      int y = start_y + row * (rect_height + spacing);

      digletts[index].x = x;
      digletts[index].y = y;
      digletts[index].width = rect_width;
      digletts[index].height = rect_height;
      digletts[index].key = 0;         // device-specific
      digletts[index].visible = false; // start with all digletts hidden
      digletts[index].active = true;

      if (index < 3) {
        digletts[index].timer = get_random_timer(10, 30);
      }
      else {
        digletts[index].timer = get_random_timer(MIN_DIGLETT_HIDE_TIME, MAX_DIGLETT_HIDE_TIME);
      }

      // draw empty hole to static buffer
      vg_draw_rectangle(x, y, rect_width, rect_height, BACKGROUND_COLOR);

      if (diglett_sprites[index])
        animated_sprite_destroy(diglett_sprites[index]);
      diglett_sprites[index] = animated_sprite_create(
        diglett_appear_frames, DIGLETT_APPEAR_NUM_FRAMES, x, y, 5);
    }
  }
  draw_background();

  int counter_width = 200;
  int counter_x = 800 - counter_width - 10;

  // clear the entire counter area
  vg_draw_rectangle(counter_x, 10, counter_width, 30, BACKGROUND_COLOR);

  // draw static "Points:" label
  draw_text_scaled("Points:", counter_x + 10, 10, 0xFFFFFF, 2);

  // switch back to back buffer for dynamic elements
  set_drawing_to_back();

  // static background to back buffer for initial display
  copy_static_to_back();

  // draw initial points value (0) directly to back buffer
  draw_points_counter();
}

void draw_background(void) {
  background_draw();

  int rect_width = 60;
  int rect_height = 80;
  int spacing = 60;
  int grid_width = 3 * rect_width + 2 * spacing;
  int start_x = (800 - grid_width) / 2;
  int start_y = 150;

  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
      int x = start_x + col * (rect_width + spacing);
      int y = start_y + row * (rect_height + spacing);

      Sprite *dirt = sprite_create_from_xpm((xpm_map_t) dirt_xpm, x, y);
      if (dirt) {
        sprite_draw(dirt);
        sprite_destroy(dirt);
      }
    }
  }
}

// update game state
void playing_update(bool is_kbd) {
  bool needs_update = false;

  // static background to back buffer
  set_drawing_to_back();
  copy_static_to_back();

  // process all digletts
  for (int i = 0; i < NUM_DIGLETTS; i++) {
    if (!digletts[i].active)
      continue;

    digletts[i].timer--;

    if (digletts[i].timer <= 0) {

      if (digletts[i].visible && digletts[i].active) {
        // currently visible diglett should hide
        digletts[i].visible = false;
        visible_diglett_count--;

        if (diglett_sprites[i]) {
          diglett_sprites[i]->x = digletts[i].x;
          diglett_sprites[i]->y = digletts[i].y;
          animated_sprite_update(diglett_sprites[i]);
          animated_sprite_draw(diglett_sprites[i]);
        }

        // timer for hidden state
        digletts[i].timer = get_random_timer(MIN_DIGLETT_HIDE_TIME, MAX_DIGLETT_HIDE_TIME);
        needs_update = true;
      }
      else if (visible_diglett_count < MAX_VISIBLE_DIGLETTS) {
        // Make the diglett appear
        digletts[i].visible = true;
        visible_diglett_count++;

        // timer for visible state
        digletts[i].timer = get_random_timer(MIN_DIGLETT_SHOW_TIME, MAX_DIGLETT_SHOW_TIME);
        needs_update = true;
      }
      else {
        // at max visible capacity, reset timer and try again later
        digletts[i].timer = get_random_timer(10, 30);
      }
    }
  }

  // Draw all digletts (both visible and hidden)
  for (int i = 0; i < NUM_DIGLETTS; i++) {
    if (digletts[i].active) {

      if (digletts[i].visible) {
        // Only draw the background rectangle if we don't have a sprite
        if (!diglett_sprites[i]) {
          vg_draw_rectangle(digletts[i].x, digletts[i].y, digletts[i].width, digletts[i].height, DIGLETT_COLOR);
        }

        // Draw animated diglett sprite
        if (diglett_sprites[i]) {
          diglett_sprites[i]->x = digletts[i].x;
          diglett_sprites[i]->y = digletts[i].y;
          animated_sprite_update(diglett_sprites[i]);
          animated_sprite_draw(diglett_sprites[i]);
        }

        if (is_kbd) {
          extern void draw_kbd_diglett_label(int index);
          draw_kbd_diglett_label(i);
        }
      }
      else {
        vg_draw_rectangle(digletts[i].x, digletts[i].y, digletts[i].width, digletts[i].height, BACKGROUND_COLOR);
      }
    }
  }

  draw_points_counter();
}

// Function to draw just a diglett without copying from static buffer
void draw_diglett(int index, bool is_kbd) {
  Diglett *dig = &digletts[index];

  if (dig->visible) {
    set_drawing_to_back();

    // Only draw background rectangle if we don't have a sprite
    if (!diglett_sprites[index]) {
      vg_draw_rectangle(dig->x, dig->y, dig->width, dig->height, DIGLETT_COLOR);
    }

    // Update and draw the sprite if available
    if (diglett_sprites[index]) {
      diglett_sprites[index]->x = dig->x;
      diglett_sprites[index]->y = dig->y;
      animated_sprite_update(diglett_sprites[index]);
      animated_sprite_draw(diglett_sprites[index]);
    }

    if (is_kbd) {
      extern void draw_kbd_diglett_label(int index);
      draw_kbd_diglett_label(index);
    }
  }
  else {
    vg_draw_rectangle(dig->x, dig->y, dig->width, dig->height, BACKGROUND_COLOR);
  }
}

void update_diglett_visibility(int index) {
  Diglett *dig = &digletts[index];

  if (dig->visible) {
    // Only draw background rectangle if we don't have a sprite
    if (!diglett_sprites[index]) {
      vg_draw_rectangle(dig->x, dig->y, dig->width, dig->height, DIGLETT_COLOR);
    }

    // Draw animated sprite
    if (diglett_sprites[index]) {
      diglett_sprites[index]->x = dig->x;
      diglett_sprites[index]->y = dig->y;
      animated_sprite_update(diglett_sprites[index]);
      animated_sprite_draw(diglett_sprites[index]);
    }
  }
  else {
    vg_draw_rectangle(dig->x, dig->y, dig->width, dig->height, BACKGROUND_COLOR);
  }
}

bool whack_diglett(int index) {
  if (index < 0 || index >= NUM_DIGLETTS) {
    return false;
  }

  if (!digletts[index].active) {
    return false;
  }

  if (digletts[index].visible) {
    // digglet visible =  successful whack
    digletts[index].visible = false;
    visible_diglett_count--;

    // new timer for diglett to reappear
    digletts[index].timer = get_random_timer(MIN_DIGLETT_HIDE_TIME, MAX_DIGLETT_HIDE_TIME);

    update_diglett_visibility(index);

    // +1 point
    if (player_points < 999) {
      player_points++;
    }

    return true;
  }
  else { // diglett hidden = unsuccessful whack
    // -1 point
    if (player_points > 0) {
      player_points--;
    }

    return false;
  }
}

// Common input handling function
void playing_handle_common_input(int index, bool is_kbd) {
  set_drawing_to_back();
  copy_static_to_back();

  whack_diglett(index);

  playing_update(is_kbd);
}

void set_playing_mode(bool is_keyboard_mode) {
  using_keyboard_mode = is_keyboard_mode;
}

void playing_destroy(void) {
  for (int i = 0; i < NUM_DIGLETTS; i++) {
    if (diglett_sprites[i]) {
      animated_sprite_destroy(diglett_sprites[i]);
      diglett_sprites[i] = NULL;
    }
  }
}
