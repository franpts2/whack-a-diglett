/**
 * @file video.h
 * @brief Header file for video controller
 *
 * This file provides functions and structures for video mode setting,
 * buffer management, and drawing operations.
 */

#ifndef _VIDEO_H_
#define _VIDEO_H_

#include <lcom/lcf.h>
#include <lcom/xpm.h>

/** @brief Video mode information structure */
vbe_mode_info_t m_info;

/** @brief Physical VRAM address */
void *video_mem;        

/** @brief Back buffer for double/triple buffering */
void *back_buffer;      

/** @brief Middle buffer for triple buffering */
void *middle_buffer;    

/** @brief Static buffer for unchanging background elements */
void *static_buffer;    

/** @brief Current buffer index (0, 1, 2) */
uint8_t current_buffer; 

/**
 * @brief Sets the video mode
 * 
 * @param mode Video mode to set
 * @return 0 on success, non-zero otherwise
 */
int(set_video_mode)(uint16_t mode);

/**
 * @brief Maps the frame buffer for a specific mode
 * 
 * @param mode Video mode to map
 * @return 0 on success, non-zero otherwise
 */
int(map_frame_buffer)(uint16_t mode);

/**
 * @brief Initializes all buffers for rendering
 * 
 * @return 0 on success, non-zero otherwise
 */
int(init_buffers)(void);

/**
 * @brief Clears the current buffer
 * 
 * Fills the current drawing buffer with zeros.
 */
void(clear_buffer)(void);

/**
 * @brief Swaps the buffers to display rendered content
 * 
 * Handles the buffer rotation for smooth animation.
 */
void(swap_buffers)(void);

/**
 * @brief Deallocates buffer memory
 * 
 * Frees memory allocated for all buffers.
 */
void(destroy_buffers)(void);

/**
 * @brief Copies the static buffer to the back buffer
 * 
 * Used for efficient rendering of static elements with dynamic overlays.
 */
void(copy_static_to_back)(void);

/**
 * @brief Sets the drawing target to the static buffer
 * 
 * Directs all subsequent drawing operations to the static buffer.
 */
void(set_drawing_to_static)(void); 

/**
 * @brief Sets the drawing target to the back buffer
 * 
 * Directs all subsequent drawing operations to the back buffer.
 */
void(set_drawing_to_back)(void);

/**
 * @brief Gets the current drawing buffer
 * 
 * @return Pointer to the current buffer for drawing operations
 */
void *get_current_buffer(void);

/**
 * @brief Calculates a color for rectangle drawing patterns
 * 
 * @param row Current row
 * @param col Current column
 * @param first First color in pattern
 * @param step Color step
 * @param n_rect Number of rectangles
 * @param is_direct Whether to use direct mode
 * @return Calculated color value
 */
uint32_t get_rectangle_color(uint8_t row, uint8_t col, uint32_t first, uint8_t step, uint8_t n_rect, bool is_direct);

/**
 * @brief Draws an XPM pixmap to the screen
 * 
 * @param xpm XPM map to draw
 * @param x X-coordinate
 * @param y Y-coordinate
 * @return 0 on success, non-zero otherwise
 */
int draw_pixmap(xpm_map_t xpm, uint16_t x, uint16_t y);

/**
 * @brief Draws a single pixel
 * 
 * @param x X-coordinate
 * @param y Y-coordinate
 * @param color Color value
 */
void draw_pixel(int x, int y, uint32_t color);

/**
 * @brief Draws a scaled pixel (multiple pixels as one)
 * 
 * @param x X-coordinate
 * @param y Y-coordinate
 * @param color Color value
 * @param scale Scale factor
 */
void draw_pixel_scaled(int x, int y, uint32_t color, int scale);

/**
 * @brief Draws a filled rectangle
 * 
 * @param x X-coordinate of top-left corner
 * @param y Y-coordinate of top-left corner
 * @param width Width of rectangle
 * @param height Height of rectangle
 * @param color Color of rectangle
 * @return 0 on success, non-zero otherwise
 */
int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

#endif
