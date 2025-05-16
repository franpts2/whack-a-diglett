#ifndef _VIDEO_H_
#define _VIDEO_H_

#include <lcom/lcf.h>

vbe_mode_info_t m_info;

int (set_video_mode)(uint16_t mode);

int (map_frame_buffer)(uint16_t mode);

void *video_mem;



#endif
