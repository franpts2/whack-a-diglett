#ifndef _VIDEO_H_
#define _VIDEO_H_

#include <lcom/lcf.h>
#include <lcom/xpm.h>

vbe_mode_info_t m_info;

// Triple buffering setup
void *video_mem;        // physical VRAM address
void *back_buffer;      // back buffer
void *middle_buffer;    // middle buffer
uint8_t current_buffer; // current buffer index (0, 1, 2)

int(set_video_mode)(uint16_t mode);

int(map_frame_buffer)(uint16_t mode);

// triple buffer functions
int(init_buffers)(void);
void(clear_buffer)(void);
void(swap_buffers)(void);
void(destroy_buffers)(void);

uint32_t get_rectangle_color(uint8_t row, uint8_t col, uint32_t first, uint8_t step, uint8_t n_rect, bool is_direct);

int draw_pixmap(xpm_map_t xpm, uint16_t x, uint16_t y);

void draw_pixel(int x, int y, uint32_t color);

void draw_pixel_scaled(int x, int y, uint32_t color, int scale);

int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

#endif
