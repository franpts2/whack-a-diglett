#ifndef _VIDEO_H_
#define _VIDEO_H_

#include <lcom/lcf.h>
#include <lcom/xpm.h>

vbe_mode_info_t m_info;

int (set_video_mode)(uint16_t mode);

int (map_frame_buffer)(uint16_t mode);

void *video_mem;

uint32_t get_rectangle_color (uint8_t row, uint8_t col, uint32_t first, uint8_t step, uint8_t n_rect, bool is_direct);

int draw_pixmap(xpm_map_t xpm, uint16_t x, uint16_t y);



#endif
