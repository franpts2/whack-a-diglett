#include <lcom/lcf.h>
#include "video.h"

int (set_video_mode)(uint16_t mode){
  reg86_t r;
  memset(&r, 0, sizeof(r));
  r.ah = 0x4F;
  r.al = 0x02;
  r.bx = mode | BIT(14);
  r.intno = 0x10;

  if (sys_int86(&r) != 0){
    printf("sys_int86 failed!\n");
    return 1;
  }

  return 0;
}

int (map_frame_buffer)(uint16_t mode){
 
  //memset(m_info, 0, sizeof(m_info));

  if (vbe_get_mode_info(mode, &m_info)) return 1;

  // static void *video_mem;         /* frame-buffer VM address (static global variable*/ 
 
  struct minix_mem_range mr;
  unsigned int bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
  unsigned int vram_base = m_info.PhysBasePtr;  /* VRAM's physical addresss */
  unsigned int vram_size = m_info.XResolution * m_info.YResolution * bytes_per_pixel; /* VRAM'size*/

  int r;				    

  /* Allow memory mapping */

  mr.mr_base = (phys_bytes) vram_base;	
  mr.mr_limit = mr.mr_base + vram_size;  

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

  if(video_mem == MAP_FAILED)
    panic("couldn't map video memory");
  
  return 0;
}


int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){
  if (x + len > m_info.XResolution || y >= m_info.YResolution) return 1;

  unsigned bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
  
  unsigned int start_pos = (y * m_info.XResolution + x) * bytes_per_pixel;

  uint8_t color_buffer[bytes_per_pixel];

  for (unsigned i = 0; i < bytes_per_pixel; i++){
    color_buffer[i] = (color >> (i*8)) & 0xFF;
  }

  for (uint16_t i = 0; i < len; i++){
    unsigned int current_pos = start_pos + (i * bytes_per_pixel);
    memcpy((uint8_t *)video_mem + current_pos, color_buffer, bytes_per_pixel);
  }

  return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
  for (unsigned row = 0; row < height; row++){
    if (vg_draw_hline(x, y + row, width, color) != 0) {
      vg_exit();
      return 1;
    }
  }
  return 0;
}

