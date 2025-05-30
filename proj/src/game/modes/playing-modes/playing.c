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
#include <game/sprites/animated_sprite.h>
#include <game/sprites/animations/diglett_appear_xpm.h>
#include <game/sprites/animations/diglett_boink_xpm.h> 
#include <game/sprites/pixelart/dirt_xpm.h>
#include <time.h>

struct timespec game_start_time = {0, 0};
extern int game_time_left;
struct timespec last_update_time = {0, 0};
struct timespec pause_start_time = {0, 0}; // Time when game was paused
double total_paused_time = 0.0; // Total time spent in pause mode

// Forward declarations
void draw_background(void);
void playing_destroy(void);

// common game variables
Diglett digletts[NUM_DIGLETTS];
int visible_diglett_count = 0;
int player_points = 0;

extern xpm_map_t diglett_appear_frames[];
extern xpm_map_t diglett_boink_frames[]; 

AnimatedSprite *diglett_sprites[NUM_DIGLETTS] = {0};
AnimatedSprite *diglett_boink_sprites[NUM_DIGLETTS] = {0}; 

bool diglett_appear_anim_done[NUM_DIGLETTS] = {0};

static bool using_keyboard_mode = true;
static bool static_buffer_initialized = false;

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

  // Reset timer variables - ensure we start with a fresh 60 seconds
  clock_gettime(CLOCK_MONOTONIC, &game_start_time);
  total_paused_time = 0.0; // Reset pause time accumulator
  game_time_left = TIMER_BAR_TOTAL_SECONDS;

  // hide cursor in kbd playing mode by moving it off-screen (it won't be rendered anyway)
  if (g_cursor != NULL) {
    cursor_set_position(g_cursor, -100, -100);
  }

  prev_mode = MODE_PLAYING; // mark transition as complete

  extern void *back_buffer;
  extern void *static_buffer;
  extern void *middle_buffer;
  unsigned int bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
  unsigned int buffer_size = m_info.XResolution * m_info.YResolution * bytes_per_pixel;

  // Always clear both buffers to ensure proper background color
  memset(static_buffer, 0, buffer_size);
  memset(back_buffer, 0, buffer_size);
  
  // Reset static_buffer_initialized to ensure all elements get redrawn when restarting game
  static_buffer_initialized = false;
  
  // Always draw the background to static buffer
  set_drawing_to_static();
  background_init(); // Initialize the background

  // Only draw the static elements if not already done
  if (!static_buffer_initialized) {
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
    int start_x = (800 - grid_width) / 2 - spacing/2;
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
        digletts[index].boinking = false; // <-- initialize boinking state
        digletts[index].boink_timer = 0;  // <-- initialize boink timer

        if (index < 3) {
          digletts[index].timer = get_random_timer(10, 30);
        }
        else {
          digletts[index].timer = get_random_timer(MIN_DIGLETT_HIDE_TIME, MAX_DIGLETT_HIDE_TIME);
        }

        // draw empty hole to static buffer
        vg_draw_rectangle(x, y, rect_width, rect_height, BACKGROUND_COLOR);

        if (diglett_sprites[index]) animated_sprite_destroy(diglett_sprites[index]);
        int frame_delay = (sys_hz() / 60 < 1) ? 1 : sys_hz() / 60;
        diglett_sprites[index] = animated_sprite_create(
          diglett_appear_frames, DIGLETT_APPEAR_NUM_FRAMES, x, y, frame_delay
        );
        // create boink sprite for each diglett
        if (diglett_boink_sprites[index]) animated_sprite_destroy(diglett_boink_sprites[index]);
        diglett_boink_sprites[index] = animated_sprite_create(
          diglett_boink_frames, DIGLETT_BOINK_NUM_FRAMES, x, y, frame_delay
        );
      }
    }
    draw_background();
    static_buffer_initialized = true;
  }

  // Always clear back buffer for new frame
  memset(back_buffer, 0, buffer_size);

  // switch back to back buffer for dynamic elements
  set_drawing_to_back();

  // Draw background (green) to back buffer
  vg_draw_rectangle(0, 0, 800, 600, BACKGROUND_COLOR);

  // static background to back buffer for initial display
  copy_static_to_back();

  // draw initial points value (0) directly to back buffer
  draw_points_counter();

  clock_gettime(CLOCK_MONOTONIC, &last_update_time);

  clock_gettime(CLOCK_MONOTONIC, &game_start_time);
  game_time_left = TIMER_BAR_TOTAL_SECONDS;
}

void draw_background(void) {
  background_draw();

  int rect_width = 60;
  int rect_height = 80;
  int spacing = 60;
  int grid_width = 3 * rect_width + 2 * spacing;
  int start_x = (800 - grid_width) / 2 - spacing/2;
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
  for (int i = 0; i < NUM_DIGLETTS; i++) {
    if (!digletts[i].active)
      continue;

    // Handle boink animation
    if (digletts[i].boinking) {
      digletts[i].boink_timer--;
      if (digletts[i].boink_timer <= 0) {
        digletts[i].boinking = false;
        digletts[i].visible = false;
        visible_diglett_count--;
        digletts[i].timer = get_random_timer(MIN_DIGLETT_HIDE_TIME, MAX_DIGLETT_HIDE_TIME);
        diglett_appear_anim_done[i] = false;
      }
      continue; // skip normal timer/visibility logic while boinking
    }

    digletts[i].timer--;

    if (digletts[i].timer <= 0) {
      if (digletts[i].visible && digletts[i].active) {
        digletts[i].visible = false;
        visible_diglett_count--;
        digletts[i].timer = get_random_timer(MIN_DIGLETT_HIDE_TIME, MAX_DIGLETT_HIDE_TIME);
        diglett_appear_anim_done[i] = false;
      }
      else if (visible_diglett_count < MAX_VISIBLE_DIGLETTS) {
        digletts[i].visible = true;
        visible_diglett_count++;
        digletts[i].timer = get_random_timer(MIN_DIGLETT_SHOW_TIME, MAX_DIGLETT_SHOW_TIME);
        diglett_appear_anim_done[i] = false; // reset flag when appearing
        if (diglett_sprites[i]) diglett_sprites[i]->current_frame = 0; // restart animation
      }
      else {
        digletts[i].timer = get_random_timer(10, 30);
      }
    }
  }

  // 3. Draw all visible digletts or boinking digletts
  for (int i = 0; i < NUM_DIGLETTS; i++) {
    if (digletts[i].active && (digletts[i].visible || digletts[i].boinking)) {
      draw_diglett(i, is_kbd);
    }
  }

  draw_points_counter();
  draw_timer_bar();
}

// Function to draw just a diglett (never clear with BACKGROUND_COLOR)
void draw_diglett(int index, bool is_kbd) {
  Diglett *dig = &digletts[index];

  set_drawing_to_back();

  if (dig->boinking && diglett_boink_sprites[index]) {
    diglett_boink_sprites[index]->x = dig->x;
    diglett_boink_sprites[index]->y = dig->y;
    animated_sprite_update(diglett_boink_sprites[index]);
    animated_sprite_draw(diglett_boink_sprites[index]);
    return;
  }

  if (!diglett_sprites[index]) {
    vg_draw_rectangle(dig->x, dig->y, dig->width, dig->height, DIGLETT_COLOR);
  }

  if (diglett_sprites[index]) {
    diglett_sprites[index]->x = dig->x;
    diglett_sprites[index]->y = dig->y;

    if (!diglett_appear_anim_done[index]) {
      animated_sprite_update(diglett_sprites[index]);
      animated_sprite_draw(diglett_sprites[index]);

      if (diglett_sprites[index]->current_frame >= (int)(DIGLETT_APPEAR_NUM_FRAMES - 1)) {
        diglett_appear_anim_done[index] = true;
      }
    } else {
      // última frame
      diglett_sprites[index]->current_frame = DIGLETT_APPEAR_NUM_FRAMES - 1;
      animated_sprite_draw(diglett_sprites[index]);
    }
  }

  if (is_kbd) {
    extern void draw_kbd_diglett_label(int index);
    draw_kbd_diglett_label(index);
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
      if (!diglett_appear_anim_done[index]) {
        animated_sprite_update(diglett_sprites[index]);
        animated_sprite_draw(diglett_sprites[index]);
        if (diglett_sprites[index]->current_frame >= (int)(DIGLETT_APPEAR_NUM_FRAMES - 1)) {
          diglett_appear_anim_done[index] = true;
        }
      } else {
        diglett_sprites[index]->current_frame = DIGLETT_APPEAR_NUM_FRAMES - 1;
        animated_sprite_draw(diglett_sprites[index]);
      }
    }
  }
  else {
    vg_draw_rectangle(dig->x, dig->y, dig->width, dig->height, BACKGROUND_COLOR);
    diglett_appear_anim_done[index] = false; 
  }
}

bool whack_diglett(int index) {
  if (index < 0 || index >= NUM_DIGLETTS) {
    return false;
  }

  if (!digletts[index].active) {
    return false;
  }

  if (digletts[index].visible && !digletts[index].boinking) {
    // diglett visible = successful whack
    // Instead of hiding, start boink animation
    digletts[index].boinking = true;
    digletts[index].boink_timer = DIGLETT_BOINK_NUM_FRAMES; // or adjust for frame delay
    if (diglett_boink_sprites[index]) diglett_boink_sprites[index]->current_frame = 0;

    // +1 point
    if (player_points < 999) {
      player_points++;
    }

    return true;
  }
  else { // diglett hidden or already boinking = unsuccessful whack
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
    if (diglett_boink_sprites[i]) { // destroy boink sprites
      animated_sprite_destroy(diglett_boink_sprites[i]);
      diglett_boink_sprites[i] = NULL;
    }
  }
}

void draw_timer_bar() {
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  
  double elapsed = (now.tv_sec - game_start_time.tv_sec) +
                   (now.tv_nsec - game_start_time.tv_nsec) / 1e9 - total_paused_time;
  
  int time_left = TIMER_BAR_TOTAL_SECONDS - (int)elapsed;
  if (time_left < 0) time_left = 0;
  game_time_left = time_left;

  // border
  vg_draw_rectangle(TIMER_BAR_X - 2, TIMER_BAR_Y - 2, TIMER_BAR_WIDTH + 4, TIMER_BAR_HEIGHT + 4, 0xFFFFFF);

  // barra vazia
  vg_draw_rectangle(TIMER_BAR_X, TIMER_BAR_Y, TIMER_BAR_WIDTH, TIMER_BAR_HEIGHT, 0x222222);

  // cor diferente dependendo do tempo que falta. talvez adicione cores intermédias?
  uint32_t bar_color;
  if (game_time_left > (2 * TIMER_BAR_TOTAL_SECONDS) / 3) {
    bar_color = 0x00FF33; // green
  } else if (game_time_left > (TIMER_BAR_TOTAL_SECONDS) / 3) {
    bar_color = 0xFFFF00; // yellow
  } else {
    bar_color = 0xFF3333; // red
  }

  // calcula e desenha tempo restante
  int fill_width = (game_time_left * TIMER_BAR_WIDTH) / TIMER_BAR_TOTAL_SECONDS;
  if (fill_width < 0) fill_width = 0;
  vg_draw_rectangle(TIMER_BAR_X, TIMER_BAR_Y, fill_width, TIMER_BAR_HEIGHT, bar_color);

  // countdown
  char buf[16];
  sprintf(buf, "%02d", game_time_left);
  draw_text_scaled(buf, TIMER_BAR_X + TIMER_BAR_WIDTH + 12, TIMER_BAR_Y - 2, 0xFFFFFF, 2);

  // gameover
  if (game_time_left == 0) {
    extern GameMode current_mode;
    current_mode = MODE_GAMEOVER;
  }
}
