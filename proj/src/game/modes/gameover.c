#include "gameover.h"
#include "../../controllers/video/video.h"
#include "../../fonts/testfont.h"
#include "../game.h"
#include <stdbool.h>
#include <stdio.h>
#include "playing/playing_kbd.h"

#define BUTTON_WIDTH 300
#define BUTTON_HEIGHT 50
#define BUTTON_SPACING 30
#define NUM_BUTTONS 2

typedef struct {
    int x, y;
    int width, height;
    char text[32];
    bool hover;
} GameOverButton;

static GameOverButton buttons[NUM_BUTTONS];
static int selected = 0;
static int prev_selected = -1;

#define GAMEOVER_BG_COLOR 0x02
#define BUTTON_COLOR 0xAAAAAA
#define BUTTON_HOVER_COLOR 0xCCCCCC
#define TEXT_COLOR 0x02
#define SELECTION_COLOR 0xFF

extern int player_points;
extern GameMode current_mode;
extern GameMode prev_mode;

void gameover_init(void) {
    int screen_center_x = 800 / 2;
    int start_y = 300;
    
    // Play Again button
    buttons[0].width = BUTTON_WIDTH;
    buttons[0].height = BUTTON_HEIGHT;
    buttons[0].x = screen_center_x - (BUTTON_WIDTH / 2);
    buttons[0].y = start_y;
    sprintf(buttons[0].text, "Play again");
    buttons[0].hover = false;
    
    // Main Menu button
    buttons[1].width = BUTTON_WIDTH;
    buttons[1].height = BUTTON_HEIGHT;
    buttons[1].x = screen_center_x - (BUTTON_WIDTH / 2);
    buttons[1].y = start_y + BUTTON_HEIGHT + BUTTON_SPACING;
    sprintf(buttons[1].text, "Return to menu");
    buttons[1].hover = false;
    
    // Reset selection
    selected = 0;
    prev_selected = -1;
    
    prev_mode = MODE_GAMEOVER;
    
    gameover_draw();
}

void gameover_draw(void) {
    // Clear screen with same background color as menu
    vg_draw_rectangle(0, 0, 800, 600, GAMEOVER_BG_COLOR);
    
    // Draw "GAME OVER" title
    int title_scale = 3;
    const char *title = "GAME OVER";
    int title_width = strlen(title) * 8 * title_scale;
    int title_x = (800 - title_width) / 2;
    draw_text_scaled(title, title_x, 100, 0xFF, title_scale);
    
    // Draw score
    char score_text[32];
    sprintf(score_text, "Your score: %d", player_points);
    int score_scale = 2;
    int score_width = strlen(score_text) * 8 * score_scale;
    int score_x = (800 - score_width) / 2;
    draw_text_scaled(score_text, score_x, 200, 0xFF, score_scale);
    
    // Draw buttons
    for (int i = 0; i < NUM_BUTTONS; i++) {
        vg_draw_rectangle(buttons[i].x, buttons[i].y, 
                        buttons[i].width, buttons[i].height, 
                        BUTTON_COLOR);
        
        int text_scale = 2;
        int text_width = strlen(buttons[i].text) * 8 * text_scale;
        int text_x = buttons[i].x + (buttons[i].width - text_width) / 2;
        int text_y = buttons[i].y + (buttons[i].height - 8 * text_scale) / 2; // 8 is half of font height (16)
        draw_text_scaled(buttons[i].text, text_x, text_y, TEXT_COLOR, text_scale);
    }
    
    int arrow_x = buttons[selected].x + buttons[selected].width + 10;
    int arrow_w = 30, arrow_h = 50;
    vg_draw_rectangle(arrow_x, buttons[selected].y, arrow_w, arrow_h, SELECTION_COLOR);
    
    prev_selected = selected;
}

void gameover_handle_input(uint8_t scancode) {
    if (scancode == 0x48) { // Up arrow key
        selected = (selected - 1 + NUM_BUTTONS) % NUM_BUTTONS;
        gameover_draw();
    }
    else if (scancode == 0x50) { // Down arrow key
        selected = (selected + 1) % NUM_BUTTONS;
        gameover_draw();
    }
    else if (scancode == 0x1C) { // Enter key
        // Execute selected action
        if (selected == 0) { // Play again
            current_mode = MODE_PLAYING;
            playing_kbd_init();
        } else { // Return to menu
            current_mode = MODE_MENU;
        }
    }
    else if (scancode == 0x01) { // ESC key
        current_mode = MODE_MENU;
    }
}




