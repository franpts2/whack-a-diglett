#ifndef GAMEOVER_H
#define GAMEOVER_H

#include <stdbool.h>
#include <stdint.h>

void gameover_init(void);
void gameover_handle_input(uint8_t scancode);
void gameover_update_selection(void);
void gameover_select_option(void);

void gameover_handle_mouse(int x, int y, bool left_button_pressed);

void draw_gameover_bg_and_buttons(void);
void draw_gameover_selection(void);

extern int gameover_selected;

#endif // GAMEOVER_H
