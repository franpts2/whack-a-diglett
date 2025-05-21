// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>
#include <lcom/xpm.h>
#include <stdint.h>
#include <stdio.h>
#include "video.h"
#include "keyboard.h"

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  
  if(set_video_mode(mode) != 0) return 1;
  sleep(delay);
  if(vg_exit() != 0) return 1;

  return 0;
}

int (esc_key)(){
  int ipc_status, r;
  message msg;
  uint8_t bit_no;

  if ((r = keyboard_subscribe_int(&bit_no)) != 0) return 1;

  while (scancode != 0x81){
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0){
      printf("driver_receive failed with: %d\n", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)){
      switch (_ENDPOINT_P(msg.m_source))
      {
      case HARDWARE:
        if (msg.m_notify.interrupts & bit_no){
          kbc_ih();
        }
        break;
      
      default:
        break;
      }
    }
  }

  if (keyboard_unsubscribe_int() != 0){
    return 1;
  }
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  
  if (map_frame_buffer(mode) != 0) return 1;

  if (set_video_mode(mode) != 0) return 1;

  if (vg_draw_rectangle(x, y, width, height, color) != 0) return 1;

  printf("Rectangle drawn. Press ESC to exit...\n");

  if (esc_key() != 0) return 1;

  if (vg_exit() != 0) return 1;

  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {

  if (map_frame_buffer(mode) != 0) return 1;

  if (set_video_mode(mode) != 0) return 1;

  uint32_t width = m_info.XResolution / no_rectangles;
  uint32_t height = m_info.YResolution / no_rectangles;
  bool is_direct = m_info.MemoryModel == 6; // 6 -> direct color

  for (uint8_t row = 0; row < no_rectangles; row++){
    for (uint8_t col = 0; col < no_rectangles; col++){
      
      uint32_t color = get_rectangle_color(row, col, first, step, no_rectangles, is_direct);

      uint16_t x = col * width;
      uint16_t y = row * height;

      if(vg_draw_rectangle(x, y, width, height, color) != 0) return 1;
    }
  }

  if (esc_key() != 0) return 1;
  
  if (vg_exit() != 0) return 1;

  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  if (map_frame_buffer(0x105) != 0) return 1;

  if (set_video_mode(0x105) != 0) return 1;

  if (draw_pixmap(xpm, x, y) != 0) return 1;

  if (esc_key() != 0) return 1;

  if (vg_exit() != 0) return 1;

  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {

  if (map_frame_buffer(0x105) != 0) return 1;
  if (set_video_mode(0x105) != 0) return 1;

  uint8_t bit_kbd, bit_timer;
  if (keyboard_subscribe_int(&bit_kbd) != 0) return 1;
  if (timer_subscribe_int(&bit_timer) != 0) return 1;
  if (timer_set_frequency(0, fr_rate) != 0) return 1;

  if (draw_pixmap(xpm, xi, yi) != 0) return 1;

  uint16_t x = xi;
  uint16_t y = yi;

  int ipc_status, r;
  message msg;

  bool vertical = xi == xf;
  // if (xi == xf && yi < yf) vertical = true;
  // else if (yi == yf && xi < xf) vertical = false;
  // else return 1;

  while (scancode != 0x81 && (x != xf || x != yf)){
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0){
      printf("driver_receive failed with: %d\n", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)){
      switch (_ENDPOINT_P(msg.m_source)){
        case HARDWARE:
          if (msg.m_notify.interrupts & bit_kbd){
            kbc_ih();
          }
          if (msg.m_notify.interrupts & bit_timer){
            xpm_image_t img;
            xpm_load(xpm, XPM_INDEXED, &img);

            for (int row = 0; row < img.width; row++){
              for (int col = 0; col < img.height; col++){
                vg_draw_rectangle(x + row, y + col, 1, 1, 0);
              }
            }

            if (vertical){
              y+= speed;
              if (y > yf) y = yf;
            } else {
              x += speed;
              if (x > xf) x = xf;
            }

            if (draw_pixmap(xpm, x, y) != 0) return 1;
            
          }
          break;
          
        default:
          break;
      }
    } else {
      // do nothing
    }

  }
  
  if (keyboard_unsubscribe_int() != 0) return 1;
  if (timer_unsubscribe_int() != 0) return 1;
  if (vg_exit() != 0) return 1;
  return 0;
}

int(video_test_controller)() {
  /* This year you do not need to implement this */
  printf("%s(): under construction\n", __func__);

  return 1;
}

