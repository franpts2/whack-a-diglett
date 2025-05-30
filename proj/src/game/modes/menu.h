#ifndef MENU_H
#define MENU_H

#include <stdbool.h>
#include <stdint.h>

void menu_init(void);
void menu_handle_input(uint8_t scancode);
void menu_update_selection(void);
void menu_select_option(void);

void menu_handle_mouse(int x, int y, bool left_button_pressed);

void init_menu_static_content(void);
void draw_menu_bg_and_buttons(void);
void draw_menu_selection(void);

extern int selected;

#endif // MENU_H
