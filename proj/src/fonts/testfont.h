#ifndef TESTFONT_H
#define TESTFONT_H

#include <stdint.h>

void draw_char(char c, int x, int y, uint32_t color);
void draw_text(const char *str, int x, int y, uint32_t color);
void draw_char_scaled(char c, int x, int y, uint32_t color, int scale);
void draw_text_scaled(const char *str, int x, int y, uint32_t color, int scale);

#endif // TESTFONT_H
