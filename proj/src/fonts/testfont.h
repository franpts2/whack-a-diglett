/**
 * @file testfont.h
 * @brief Header file for bitmap font rendering
 *
 * This file provides functions to render text using a bitmap font.
 */

#ifndef TESTFONT_H
#define TESTFONT_H

#include <stdint.h>

/**
 * @brief Draws a single character
 * 
 * @param c Character to draw
 * @param x X-coordinate of top-left corner
 * @param y Y-coordinate of top-left corner
 * @param color Color to use for the character
 */
void draw_char(char c, int x, int y, uint32_t color);

/**
 * @brief Draws a string of text
 * 
 * @param str String to draw
 * @param x X-coordinate of top-left corner of first character
 * @param y Y-coordinate of top-left corner of first character
 * @param color Color to use for the text
 */
void draw_text(const char *str, int x, int y, uint32_t color);

/**
 * @brief Draws a single character with scaling
 * 
 * @param c Character to draw
 * @param x X-coordinate of top-left corner
 * @param y Y-coordinate of top-left corner
 * @param color Color to use for the character
 * @param scale Scale factor for rendering
 */
void draw_char_scaled(char c, int x, int y, uint32_t color, int scale);

/**
 * @brief Draws a string of text with scaling
 * 
 * @param str String to draw
 * @param x X-coordinate of top-left corner of first character
 * @param y Y-coordinate of top-left corner of first character
 * @param color Color to use for the text
 * @param scale Scale factor for rendering
 */
void draw_text_scaled(const char *str, int x, int y, uint32_t color, int scale);

#endif // TESTFONT_H
