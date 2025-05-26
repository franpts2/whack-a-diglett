#ifndef MENU_H
#define MENU_H

#include <stdint.h>

void menu_init(void);
void menu_handle_input(uint8_t scancode);
void menu_update_selection(void);

extern int selected;

#endif // MENU_H
