/**
 * @file game.c
 * @brief Implementation of the main game loop and state management
 *
 * Controls the main game loop, handles interrupts, and manages transitions
 * between different game modes.
 */

#include "game.h"
#include "../controllers/kbdmouse/keyboard.h"
#include "../controllers/timer/timer.h"
#include "background.h"
#include "cursor/cursor.h"
#include "modes/menu.h"
#include "modes/pause.h"
#include "modes/playing/playing_kbd.h"
#include "modes/gameover.h"
#include "modes/instructions.h"
#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdio.h>
#include "../controllers/kbdmouse/aux.h"
#include "../controllers/video/video.h"
#include "../fonts/testfont.h"

/** @brief Number of bytes per pixel in current video mode */
unsigned int bytes_per_pixel;

/** @brief Size of video buffer in bytes */
unsigned int buffer_size;

/** @brief Current game mode */
GameMode current_mode = MODE_MENU;

/** @brief Previous game mode for detecting mode changes */
GameMode prev_mode = -1;

/** @brief Previously selected menu item */
int prev_selected = -1;

/** @brief Flag indicating if the game is running */
int running = 1;

/** @brief Global cursor object used across all game modes */
Cursor *g_cursor = NULL;

/** @brief External timer counter */
extern int counter;

/** @brief Target frame rate for the game */
#define FRAME_RATE 60

/** @brief Number of timer ticks per frame */
#define TICKS_PER_FRAME (sys_hz() / FRAME_RATE)

/** @brief Counter for frame timing */
uint32_t frame_timer = 0;

/** @brief Flag indicating if a frame should be rendered */
bool render_frame = false;

/** @brief Tracks the previous state of the left mouse button */
static bool prev_left_button_state = false;

/** @brief Total game time in seconds */
#define GAME_TIME_SECONDS 60

/** @brief Remaining game time in seconds */
int game_time_left = GAME_TIME_SECONDS;

/** @brief Counter for tracking game time */
static unsigned int game_timer_counter = 0;

/** @brief Counter for tracking frame rate */
static unsigned int frame_count = 0;

/** @brief Time of last FPS calculation */
static time_t last_fps_time = 0;

/**
 * @brief Main game loop function
 *
 * Initializes game components, subscribes to interrupts, and runs the
 * main event loop that processes timer, keyboard, and mouse events.
 * Handles transitions between different game modes and renders frames
 * as needed.
 *
 * @return 0 on success, non-zero on failure
 */
int game_main_loop(void) {
  // Initialize game background
  if (background_init() != 0) {
    printf("Failed to initialize background\n");
    return 1;
  }

  // Initialize cursor
  g_cursor = cursor_init();
  if (g_cursor == NULL) {
    printf("Failed to initialize cursor\n");
    background_destroy();
    return 1;
  }

  // Subscribe to keyboard interrupts
  uint8_t kbd_irq;
  if (keyboard_subscribe_int(&kbd_irq) != 0) {
    printf("Failed to subscribe keyboard interrupt\n");
    cursor_destroy(g_cursor);
    background_destroy();
    return 1;
  }

  // Subscribe to mouse interrupts
  uint8_t mouse_irq;
  if (mouse_subscribe_int(&mouse_irq) != 0) {
    printf("Failed to subscribe mouse interrupt\n");
    keyboard_unsubscribe_int();
    cursor_destroy(g_cursor);
    background_destroy();
    return 1;
  }

  // Enable mouse data reporting
  if (my_mouse_enable_data_reporting() != 0) {
    printf("Failed to enable mouse data reporting\n");
    mouse_unsubscribe_int();
    keyboard_unsubscribe_int();
    cursor_destroy(g_cursor);
    background_destroy();
    return 1;
  }

  // Subscribe to timer interrupts
  uint8_t timer_irq;
  if (timer_subscribe_int(&timer_irq) != 0) {
    printf("Failed to subscribe timer interrupt\n");
    mouse_disable_data_reporting();
    mouse_unsubscribe_int();
    keyboard_unsubscribe_int();
    cursor_destroy(g_cursor);
    background_destroy();
    return 1;
  }

  // Set timer frequency for smooth animations
  timer_set_frequency(0, 60);
  last_fps_time = time(NULL);

  // Main event loop
  int ipc_status;
  message msg;
  while (running) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0) {
      printf("driver_receive failed\n");
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          // Handle timer interrupts
          if (msg.m_notify.interrupts & timer_irq) {
            timer_int_handler();
            frame_timer++;
            if (frame_timer >= TICKS_PER_FRAME) {
              if (prev_mode == current_mode) {
                render_frame = true;
                
                // FPS calculation
                frame_count++;
                time_t now = time(NULL);
                if (now != last_fps_time) {
                  printf("FPS: %u\n", frame_count);
                  frame_count = 0;
                  last_fps_time = now;
                }
                
                // Game timer update (only in playing mode)
                if (current_mode == MODE_PLAYING) {
                  game_timer_counter++;
                  if (game_timer_counter >= sys_hz()) {
                    if (game_time_left > 0) game_time_left--;
                    game_timer_counter = 0;
                    if (game_time_left == 0) {
                      current_mode = MODE_GAMEOVER;
                    }
                  }
                  
                  playing_kbd_update();
                  swap_buffers();
                }
              }
              frame_timer = 0;
            }
          }
          
          // Handle keyboard interrupts
          if (msg.m_notify.interrupts & kbd_irq) {
            kbc_ih();
            extern uint8_t scancode;
            if ((scancode & 0x80) == 0) {
              // Handle input based on current game mode
              switch (current_mode) {
                case MODE_MENU:
                  menu_handle_input(scancode);
                  break;
                case MODE_PLAYING:
                  playing_handle_input(scancode);
                  break;
                case MODE_PAUSED:
                  pause_handle_input(scancode);
                  break;
                case MODE_GAMEOVER:
                  gameover_handle_input(scancode);
                  break;
                case MODE_INSTRUCTIONS:
                  instructions_handle_input(scancode);
                  break;
                default:
                  break;
              }
            }
            // Exit on ESC key release
            if (scancode == 0x81)
              running = 0;
          }
          
          // Handle mouse interrupts
          if (msg.m_notify.interrupts & mouse_irq) {
            mouse_ih();
            mouse_collect_packet_byte();
            if (byte_index == 3) {
              assemble_mouse_packet();
              
              // Update cursor position in menu or gameover modes
              if (g_cursor != NULL && (current_mode == MODE_MENU || current_mode == MODE_GAMEOVER)) {
                cursor_handle_mouse_packet(g_cursor, &mouse_packet);
                bool left_button_pressed = mouse_packet.lb;
                bool left_button_clicked = left_button_pressed && !prev_left_button_state;
                prev_left_button_state = left_button_pressed;
                
                // Handle mouse input based on current mode
                if (current_mode == MODE_MENU) {
                  menu_handle_mouse(g_cursor->x, g_cursor->y, left_button_clicked);
                }
                else if (current_mode == MODE_GAMEOVER) {
                  gameover_handle_mouse(g_cursor->x, g_cursor->y, left_button_clicked);
                }
              }
              
              // Reset byte index for next packet
              byte_index = 0;
            }
          }
          break;
        default:
          break;
      }
    }

    // Handle mode changes and initialize new mode
    if (current_mode != prev_mode) {
      // Store the current mode
      GameMode new_mode = current_mode;
      switch (new_mode) {
        case MODE_MENU:
          // Reset menu and selection
          menu_init();
          set_drawing_to_static();
          
          // Clear static buffer and draw static content
          bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
          buffer_size = m_info.XResolution * m_info.YResolution * bytes_per_pixel;
          memset(static_buffer, 0, buffer_size);
          draw_menu_bg_and_buttons();
          set_drawing_to_back();
          
          // Force a render frame
          render_frame = true;
          break;
          
        case MODE_PLAYING:
          // Clear the display completely
          bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
          buffer_size = m_info.XResolution * m_info.YResolution * bytes_per_pixel;
          
          // Reset both buffers
          memset(static_buffer, 0, buffer_size);
          memset(back_buffer, 0, buffer_size);
          
          // Initialize game
          playing_kbd_init();
          set_drawing_to_back();
          
          // Reset the game timer
          game_time_left = GAME_TIME_SECONDS;
          game_timer_counter = 0;
          render_frame = true;
          swap_buffers();
          break;
          
        case MODE_PAUSED:
          pause_init();
          render_frame = true;
          break;
          
        case MODE_GAMEOVER:
          // reset gameover and selection
          gameover_init();
          
          set_drawing_to_static();

          // clear static buffer and draw static content
          bytes_per_pixel = (m_info.BitsPerPixel + 7) / 8;
          buffer_size = m_info.XResolution * m_info.YResolution * bytes_per_pixel;
          memset(static_buffer, 0, buffer_size);

          draw_gameover_bg_and_buttons();
          set_drawing_to_back();
          
          render_frame = true;
          break;
          
        case MODE_INSTRUCTIONS:
          instructions_init();
          render_frame = true;
          break;
          
        default:
          break;
      }
      prev_mode = current_mode;
    }

    // Check for game over before rendering
    if (current_mode == MODE_PLAYING && game_time_left == 0) {
      current_mode = MODE_GAMEOVER;
      render_frame = true;
      continue; // Process mode change in next iteration
    }

    // Handle rendering based on mouse movement and frame timer
    static bool mouse_moved_recently = false;
    bool mouse_moving_now = (mouse_packet.delta_x != 0 || mouse_packet.delta_y != 0);
    
    if (mouse_moving_now) {
      // Process mouse movement in menu or gameover screen
      if (current_mode == MODE_MENU) {
        copy_static_to_back();
        draw_menu_selection();
        if (g_cursor != NULL) {
          cursor_draw(g_cursor);
        }
        swap_buffers(); // Swap immediately for low latency
        mouse_moved_recently = true;
        render_frame = false; // Reset frame timer
      }
      else if (current_mode == MODE_GAMEOVER) {
        copy_static_to_back();
        draw_gameover_selection();
        if (g_cursor != NULL) {
          cursor_draw(g_cursor);
        }
        swap_buffers(); // swap immediately for low latency
        mouse_moved_recently = true;
        render_frame = false; // reset frame timer
      }
    }
    // Regular rendering (not actively moving mouse)
    else if (render_frame || mouse_moved_recently) {
      if (current_mode == MODE_MENU) {
        copy_static_to_back();
        draw_menu_selection();
        // Draw cursor on top
        if (g_cursor != NULL) {
          cursor_draw(g_cursor);
        }
        swap_buffers();
      }
      else if (current_mode == MODE_GAMEOVER) {
        copy_static_to_back();
        draw_gameover_selection();
        if (g_cursor != NULL) {
          cursor_draw(g_cursor);
        }
        swap_buffers();
      }
      else if (current_mode == MODE_INSTRUCTIONS) {
        draw_instructions();
        swap_buffers();
      }
      render_frame = false;
      
      // Reset mouse movement flag after a few frames
      if (mouse_moved_recently && !mouse_moving_now) {
        static int post_movement_frames = 0;
        if (++post_movement_frames >= 2) {
          mouse_moved_recently = false;
          post_movement_frames = 0;
        }
      }
    }
  }

  // Cleanup resources
  mouse_disable_data_reporting();
  mouse_unsubscribe_int();
  keyboard_unsubscribe_int();
  timer_unsubscribe_int();
  background_destroy();
  cursor_destroy(g_cursor);
  
  if (current_mode == MODE_INSTRUCTIONS) {
    instructions_destroy();
  }
  
  destroy_buffers();
  return 0;
}
