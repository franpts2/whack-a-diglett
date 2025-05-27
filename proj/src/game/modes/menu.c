#include "menu.h"
#include "../../controllers/video/video.h" 
#include "../../controllers/kbdmouse/keyboard.h"
#include "../../fonts/testfont.h"

#define MENU_ITEMS 3

int selected = 0; // Botão selecionado. 0 - Start Game, 1 - Instructions, 2 - Exit
static int prev_selected = 0;

void menu_handle_input(uint8_t scancode) {
    if (scancode == 0x48) { // Up
        selected = (selected - 1 + MENU_ITEMS) % MENU_ITEMS;
    } else if (scancode == 0x50) { // Down
        selected = (selected + 1) % MENU_ITEMS;
    }
}

// Desenha as coisas que não precisam de refresh (botões fundo etc)
void draw_menu_bg_and_buttons(void) {
    vg_draw_rectangle(0, 0, 1024, 768, 0x02);

    int screen_w = 1024;
    int btn_w = 300, btn_h = 50;
    int btn_x = (screen_w - btn_w) / 2; // Centrar
    int btn_y[MENU_ITEMS] = { 250, 320, 390 };
    const char* btn_labels[MENU_ITEMS] = { "Start Game", "Instructions", "Exit" };

    for (int i = 0; i < MENU_ITEMS; ++i) {
        vg_draw_rectangle(btn_x, btn_y[i], btn_w, btn_h, 0xAAAAAA); 
        int scale = 2;
        int text_width = strlen(btn_labels[i]) * 8 * scale;
        int text_x = btn_x + (btn_w - text_width) / 2;
        int text_y = btn_y[i] + (btn_h - 8 * scale) / 2;
        draw_text_scaled(btn_labels[i], text_x, text_y, 0x02, scale); 
    }
}

// Desenha o retangulo de seleção pq n sei fazer triangulos (skull emoji)
void draw_menu_selection(void) {
    int screen_w = 1024;
    int btn_w = 300;
    int btn_x = (screen_w - btn_w) / 2; 
    int btn_y[MENU_ITEMS] = { 250, 320, 390 };
    int arrow_x = btn_x + btn_w + 10;
    int arrow_w = 30, arrow_h = 50;

    // Isto "apaga" a seleção anterior. Mudar a cor caso se mude o background.
    vg_draw_rectangle(arrow_x, btn_y[prev_selected], arrow_w, arrow_h, 0x02);

    // Nova seleção
    vg_draw_rectangle(arrow_x, btn_y[selected], arrow_w, arrow_h, 0xFF); 

    prev_selected = selected;
}

// Chamado 1 vez quando o menu é loaded
void menu_init(void) {
    draw_menu_bg_and_buttons();
    draw_menu_selection();
}

// Chamado sempre que atualizarmos o selected
void menu_update_selection(void) {
    draw_menu_selection();
}
