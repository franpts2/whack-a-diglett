#ifndef _CHOOSE_MODE_H
#define _CHOOSE_MODE_H

#include "../game.h"
#include <stdbool.h>
#include <stdint.h>

extern int mode_selected;

// Function declarations for the mode selection screen
void choose_mode_init(void);
void choose_mode_handle_input(uint8_t scancode);
void draw_choose_mode_bg_and_buttons(void);
void choose_mode_update_selection(void);
void choose_mode_handle_mouse(int x, int y, bool left_button_clicked);
void choose_mode_select_option(void);

#endif // _CHOOSE_MODE_H
