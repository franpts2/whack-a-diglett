#include "video.h"
#include <lcom/lcf.h>
#include <lcom/xpm.h>
#include "../kbdmouse/keyboard.h"

// Global to track where we're currently drawing
static void *current_drawing_buffer = NULL;

// Function to expose the current drawing buffer to other modules
void *get_current_buffer(void) {
  return (current_drawing_buffer != NULL) ? current_drawing_buffer : back_buffer;
}

int(set_video_mode)(uint16_t mode) {
  reg86_t r;
  memset(&r, 0, sizeof(r));
  r.ah = 0x4F;
  r.al = 0x02;
  r.bx = mode | BIT(14);
  r.intno = 0x10;

  if (sys_int86(&r) != 0) {
    printf("sys_int86 failed!\n");
    return 1;
  }

  return 0;
}

int(map_frame_buffer)(uint16_t mode) {

  // memset(m_info, 0, sizeof(m_info));

  if (vbe_get_mode_info(mode, &m_info))
    return 1;

  // static void *video_mem;         /* frame-buffer VM address (static global variable*/

  struct minix_mem_range mr;
  unsigned int bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
  unsigned int vram_base = m_info.PhysBasePtr;                                        /* VRAM's physical addresss */
  unsigned int vram_size = m_info.XResolution * m_info.YResolution * bytes_per_pixel; /* VRAM'size*/

  int r;

  /* Allow memory mapping */

  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */

  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

  if (video_mem == MAP_FAILED)
    panic("couldn't map video memory");

  // Initialize triple buffering
  if (init_buffers() != 0)
    panic("couldn't initialize triple buffering");

  return 0;
}

int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  if (x + len > m_info.XResolution || y >= m_info.YResolution)
    return 1;

  // If current_drawing_buffer is NULL, default to back_buffer
  void *target_buffer = (current_drawing_buffer != NULL) ? current_drawing_buffer : back_buffer;

  unsigned bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;

  unsigned int start_pos = (y * m_info.XResolution + x) * bytes_per_pixel;

  uint8_t color_buffer[bytes_per_pixel];

  for (unsigned i = 0; i < bytes_per_pixel; i++) {
    color_buffer[i] = (color >> (i * 8)) & 0xFF;
  }

  for (uint16_t i = 0; i < len; i++) {
    unsigned int current_pos = start_pos + (i * bytes_per_pixel);
    memcpy((uint8_t *) target_buffer + current_pos, color_buffer, bytes_per_pixel);
  }

  return 0;
}

int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  for (unsigned row = 0; row < height; row++) {
    if (vg_draw_hline(x, y + row, width, color) != 0) {
      vg_exit();
      return 1;
    }
  }
  return 0;
}

uint32_t get_rectangle_color(uint8_t row, uint8_t col, uint32_t first, uint8_t step, uint8_t n_rect, bool is_direct) {

  if (!is_direct) {
    // indexed color mode
    return (first + (row * n_rect + col) * step) % (1 << m_info.BitsPerPixel);
  }
  else {
    // direct color mode
    uint8_t red_mask = m_info.RedMaskSize;
    uint8_t green_mask = m_info.GreenMaskSize;
    uint8_t blue_mask = m_info.BlueMaskSize;

    uint8_t red_pos = m_info.RedFieldPosition;
    uint8_t green_pos = m_info.GreenFieldPosition;
    uint8_t blue_pos = m_info.BlueFieldPosition;

    uint32_t r_first = (((1 << red_mask) - 1) & (first >> red_pos));
    uint32_t g_first = (((1 << green_mask) - 1) & (first >> green_pos));
    uint32_t b_first = (((1 << blue_mask) - 1) & (first >> blue_pos));

    uint32_t red = (r_first + col * step) % (1 << red_mask);
    uint32_t green = (g_first + row * step) % (1 << green_mask);
    uint32_t blue = (b_first + (col + row) * step) % (1 << blue_mask);

    return (red << red_pos) | (green << green_pos) | (blue << blue_pos);
  }
}

int draw_pixmap(xpm_map_t xpm, uint16_t x, uint16_t y) {

  xpm_image_t img;
  uint32_t *pixmap = (uint32_t *) xpm_load(xpm, XPM_8_8_8_8, &img);
  if (!pixmap)
    return 1;

  // Get the current drawing buffer
  uint8_t *current_buffer = (uint8_t *)get_current_buffer();

  for (uint16_t row = 0; row < img.height; row++) {
    for (uint16_t col = 0; col < img.width; col++) {
      uint32_t color = pixmap[row * img.width + col];
      draw_pixel(x + col, y + row, color, current_buffer);
    }
  }
  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {

  if (map_frame_buffer(0x115) != 0)
    return 1;
  if (set_video_mode(0x115) != 0)
    return 1;

  uint8_t bit_kbd, bit_timer;
  if (keyboard_subscribe_int(&bit_kbd) != 0)
    return 1;
  if (timer_subscribe_int(&bit_timer) != 0)
    return 1;
  if (timer_set_frequency(0, fr_rate) != 0)
    return 1;

  if (draw_pixmap(xpm, xi, yi) != 0)
    return 1;

  uint16_t x = xi;
  uint16_t y = yi;

  bool stop = false;

  int ipc_status, r;
  message msg;

  bool vertical = (xi == xf);

  while (scancode != 0x81) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d\n", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & bit_kbd) {
            kbc_ih();
          }
          if (msg.m_notify.interrupts & bit_timer) {
            timer_int_handler();

            if (!stop) {
              xpm_image_t img;
              xpm_load(xpm, XPM_INDEXED, &img);

              for (int row = 0; row < img.width; row++) {
                for (int col = 0; col < img.height; col++) {
                  vg_draw_rectangle(x + row, y + col, 1, 1, 0);
                }
              }

              if (vertical) {
                y += speed;
                if (y > yf)
                  y = yf;
              }
              else {
                x += speed;
                if (x > xf)
                  x = xf;
              }

              if (draw_pixmap(xpm, x, y) != 0)
                return 1;

              if (x == xf && y == yf)
                stop = true;
            }
          }
          break;

        default:
          break;
      }
    }
    else {
      // do nothing
    }
  }

  if (keyboard_unsubscribe_int() != 0)
    return 1;
  if (timer_unsubscribe_int() != 0)
    return 1;
  if (vg_exit() != 0)
    return 1;
  return 0;
}

int (draw_pixel)(uint16_t x, uint16_t y, uint32_t color, uint8_t *buffer) {
  if (x >= m_info.XResolution || y >= m_info.YResolution) return 1;

  // If buffer is NULL, use the current drawing buffer or back_buffer as fallback
  uint8_t *target_buffer = buffer;
  if (target_buffer == NULL) {
    target_buffer = current_drawing_buffer != NULL ? 
                    (uint8_t *)current_drawing_buffer : 
                    (uint8_t *)back_buffer;
  }

  unsigned int bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
  unsigned int index = (x + m_info.XResolution * y) * bytes_per_pixel;

  if (memcpy(&target_buffer[index], &color, bytes_per_pixel) == NULL) return 1;

  return 0;
}

void draw_pixel_scaled(int x, int y, uint32_t color, int scale) {
  vg_draw_rectangle(x, y, scale, scale, color);
}

//======= Triple buffer implementation =======

int(init_buffers)(void) {
  current_buffer = 0;

  // buffer size
  unsigned int bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
  unsigned int buffer_size = m_info.XResolution * m_info.YResolution * bytes_per_pixel;

  back_buffer = malloc(buffer_size);
  middle_buffer = malloc(buffer_size);
  static_buffer = malloc(buffer_size);

  if (back_buffer == NULL || middle_buffer == NULL || static_buffer == NULL) {
    printf("Failed to allocate memory for buffers\n");
    if (back_buffer != NULL)
      free(back_buffer);
    if (middle_buffer != NULL)
      free(middle_buffer);
    if (static_buffer != NULL)
      free(static_buffer);
    return 1;
  }

  // clear buffers initially
  memset(back_buffer, 0, buffer_size);
  memset(middle_buffer, 0, buffer_size);
  memset(static_buffer, 0, buffer_size);

  // initial drawing buffer to back buffer
  current_drawing_buffer = back_buffer;

  return 0;
}

void(clear_buffer)(void) {
  // buffer size
  unsigned int bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
  unsigned int buffer_size = m_info.XResolution * m_info.YResolution * bytes_per_pixel;

  // clear the current drawing buffer (or back buffer if none is set)
  void *target_buffer = (current_drawing_buffer != NULL) ? current_drawing_buffer : back_buffer;
  memset(target_buffer, 0, buffer_size);
}

void(swap_buffers)(void) {
  unsigned int bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
  unsigned int buffer_size = m_info.XResolution * m_info.YResolution * bytes_per_pixel;

  // back buffer to video memory (display)
  memcpy(video_mem, back_buffer, buffer_size);

  void *temp = back_buffer;
  back_buffer = middle_buffer;
  middle_buffer = static_buffer;
  static_buffer = temp;

  current_drawing_buffer = back_buffer;
  current_buffer = (current_buffer + 1) % 3;
}

void(destroy_buffers)(void) {
  if (back_buffer != NULL) {
    free(back_buffer);
    back_buffer = NULL;
  }

  if (middle_buffer != NULL) {
    free(middle_buffer);
    middle_buffer = NULL;
  }

  if (static_buffer != NULL) {
    free(static_buffer);
    static_buffer = NULL;
  }
}

//=== Static buffer management ===

void(set_drawing_to_static)(void) {
  current_drawing_buffer = static_buffer;
}

void(set_drawing_to_back)(void) {
  current_drawing_buffer = back_buffer;
}

void(copy_static_to_back)(void) {
  // swap to avoid the expensive copy
  void *temp = back_buffer;
  back_buffer = static_buffer;
  static_buffer = temp;
  current_drawing_buffer = back_buffer;
}
