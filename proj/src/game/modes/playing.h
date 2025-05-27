#ifndef _PLAYING_H
#define _PLAYING_H

#include "../game.h"
#include <stdbool.h>
#include <stdint.h>

#define BACKGROUND_COLOR 0x325918
#define DIGLETT_COLOR 0x885500

// Game timing constants
#define MIN_DIGLETT_SHOW_TIME 120 // min frames a diglett stays visible (2 second at 60Hz)
#define MAX_DIGLETT_SHOW_TIME 240 // max frames a diglett stays visible (4 seconds at 60Hz)
#define MIN_DIGLETT_HIDE_TIME 120 // min frames a diglett stays hidden (2 second at 60Hz)
#define MAX_DIGLETT_HIDE_TIME 180 // max frames a diglett stays hidden (3 seconds at 60Hz)
#define MAX_VISIBLE_DIGLETTS 5    // max digletts visible at once

typedef struct {
  int x;        // x position of the rectangle
  int y;        // y position of the rectangle
  int width;    // width of the rectangle
  int height;   // height of the rectangle
  uint8_t key;  // keyboard scancode for this diglett
  bool visible; // whether the diglett is currently visible
  int timer;    // countdown timer for state changes
  bool active;  // whether the diglett is in play (for game progression)
} Diglett;

#define NUM_DIGLETTS 9
extern Diglett digletts[NUM_DIGLETTS];
extern int visible_diglett_count;
extern int player_points;

// common game functions
void playing_init(void);
void playing_update(void);
void draw_diglett(int index);
void draw_points_counter(void);
bool whack_diglett(int index);

// helper functions
int get_random_timer(int min, int max);


#endif // _PLAYING_H
