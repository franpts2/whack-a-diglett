#ifndef MENU_H
#define MENU_H

#include <stdbool.h>
#include <stdint.h>

void menu_init(void);
void menu_handle_input(uint8_t scancode);
void menu_update_selection(void);
void menu_select_option(void);

void menu_handle_mouse(int x, int y, bool left_button_pressed);

// added these functions so they can be called from game.c
void draw_menu_bg_and_buttons(void);
void draw_menu_selection(void);

extern int selected;

#endif // MENU_H
