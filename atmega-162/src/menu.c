#include "menu.h"
#include "types.h"
#include "UI.h"
#include "joystick.h"
#include "drivers/IO_board.h"
#include "sprites.h"
#include "inter.h"

#include <stddef.h>
#include <avr/pgmspace.h>

// Menu system
Menu* current_menu = NULL;

// Store menu items in flash memory (single declaration)
const MenuItem PROGMEM submenu_1_items[] = {
    {"..", go_back_to_parent, NULL, 31},
    {"Submenu 1-1", NULL, NULL, 255},
    {"Submenu 1-2", NULL, NULL, 255},
    {"Submenu 1-3", NULL, NULL, 255},
};

const MenuItem PROGMEM submenu_2_items[] = {
    {"..", go_back_to_parent, NULL, 31},
    {"Submenu 2-1", NULL, NULL, 255},
    {"Submenu 2-2", NULL, NULL, 255},
};

const MenuItem PROGMEM main_menu_items[] = {
    {"Joystick Cal.", action_calibration_joystick, NULL, 46},
    {"Debug IO-board", action_debug_window, NULL, 15},
    {"Debug Blue Box", action_placeholder_4, NULL, 14},
    {"Settings >", NULL, &submenu_1, 30},
    {"Options >", NULL, &submenu_2, 30},
    {"Menu Item 6", NULL, NULL, 255},
    {"Menu Item 7", NULL, NULL, 255},
    {"Menu Item 8", NULL, NULL, 255},
    {"Menu Item 9", NULL, NULL, 255},
    {"Menu Item 10", NULL, NULL, 255}
};

// Menu structures (only state in RAM, items point to PROGMEM)
Menu submenu_1 = {
    .selected_item = 0,
    .total_items = 4,
    .scroll_offset = 0,
    .items = submenu_1_items,
    .parent_menu = NULL,
};

Menu submenu_2 = {
    .selected_item = 0,
    .total_items = 3,
    .scroll_offset = 0,
    .items = submenu_2_items,
    .parent_menu = NULL,
};

Menu main_menu = {
    .selected_item = 0,
    .total_items = 10,
    .scroll_offset = 0,
    .items = main_menu_items,
    .parent_menu = NULL,
};

// Cursor interaction
void update_cursor_position(void) {
    signed char dx = joystick_pos.X >> 8;
    signed char dy = joystick_pos.Y >> 8;

    const int deadzone = 3;

    if (dx > deadzone || dx < -deadzone)
        cursor_state.x += dx / 32;
    if (dy > deadzone || dy < -deadzone)
        cursor_state.y -= dy / 32;

    if (cursor_state.x < -4) cursor_state.x = -4;
    if (cursor_state.x > 124) cursor_state.x = 124;
    if (cursor_state.y < -4) cursor_state.y = -4;
    if (cursor_state.y > 60) cursor_state.y = 60;
    
    cursor_state.clicking = buttons.R6;
}

unsigned char is_cursor_over_item(unsigned char item_visual_index) {
    const signed char item_x = 8;
    const signed char item_y = 0 + (item_visual_index * 17);
    const signed char item_width = 114;
    const signed char item_height = 17;

    const signed char cx = cursor_state.x + 4; // Center of the drawn cursor
    const signed char cy = cursor_state.y + 4; // Center of the drawn cursor
    
    return (cx >= item_x && cx < item_x + item_width &&
            cy >= item_y && cy < item_y + item_height);
}

// Helper function to go back to parent menu
void go_back_to_parent(void) {
    if (current_menu != NULL && current_menu->parent_menu != NULL) {
        current_menu = current_menu->parent_menu;
    }
}

// --- Actions (transitions removed → instant screen switch) ---
void action_debug_window(void) {
    current_screen = SCREEN_DEBUG_IO_BOARD;
}

void action_placeholder_2(void) {
    // Action for menu item 2
}

void action_placeholder_3(void) {
    // Action for menu item 3
}

void action_placeholder_4(void) {
    current_screen = SCREEN_DEBUG_BLUE_BOX;
}

void action_calibration_joystick(void) {
    current_screen = SCREEN_CALIBRATION_JOYSTICK;
}

// Helper function to enter a submenu
void enter_submenu(Menu* submenu) {
    if (submenu != NULL) {
        submenu->parent_menu = current_menu;
        submenu->selected_item = 0;
        submenu->scroll_offset = 0;
        current_menu = submenu;
    }
}

// Helper function to scroll menu up
void menu_scroll_up(void) {
    if (current_menu == NULL) return;
    
    if (current_menu->selected_item > 0) {
        current_menu->selected_item--;
    } else if (current_menu->scroll_offset > 0) {
        current_menu->scroll_offset--;
    }
}

// Helper function to scroll menu down
void menu_scroll_down(void) {
    if (current_menu == NULL) return;
    
    unsigned char absolute_selected = current_menu->scroll_offset + current_menu->selected_item;
    
    if (absolute_selected < current_menu->total_items - 1) {
        if (current_menu->selected_item < 4 - 1) {
            current_menu->selected_item++;
        } else {
            current_menu->scroll_offset++;
        }
    }
}

// Draw cursor for menu
void draw_menu_cursor(void) {
    signed char dx = joystick_pos.X >> 8;
    signed char dy = joystick_pos.Y >> 8;
    unsigned char abs_dx = dx < 0 ? -dx : dx;
    unsigned char abs_dy = dy < 0 ? -dy : dy;
    unsigned char speed = abs_dx + abs_dy;

    unsigned char tile_index;
    if (speed > 150)
        tile_index = 3;
    else if (speed > 100)
        tile_index = 2;
    else if (speed > 70)
        tile_index = 1;
    else
        tile_index = 0;

    unsigned char base_tile = 6 + tile_index;

    if (buttons.R6) {
        fetch_tile_from_tilemap_2bpp(10);
    } else {
        fetch_tile_from_tilemap_2bpp(base_tile);
    }
    draw_tile_2bpp((unsigned char)cursor_state.x, (unsigned char)cursor_state.y);
}

char menu_control_mode = 0;

// Helper function to read MenuItem from PROGMEM
void read_menu_item(const MenuItem* flash_item, MenuItem* ram_item) {
    memcpy_P(ram_item, flash_item, sizeof(MenuItem));
}

void draw_menu_item(unsigned char x, unsigned char y, const char* label, unsigned char icon_tile) {
    unsigned char text_x = x;
    
    // Draw icon if it exists
    if (icon_tile != 255) {
        fetch_tile_from_tilemap_1bpp(icon_tile);
        draw_tile_1bpp(x, y);
        text_x = x + 10;  // Shift text right to make room for icon
    }
    
    // Draw text
    draw_printf(text_x, y, "%s", label);
}

void draw_menu(void) {
    if (current_menu == NULL) return;
    
    // Input detection consolidated
    static struct {
        signed char x, y;
        unsigned char nav, R5, R6, cursor_click, NU, ND, NB;
    } prev = {0};
    
    signed char old_x = cursor_state.x;
    signed char old_y = cursor_state.y;
    update_cursor_position();
    
    // Mode switching
    if (cursor_state.x != old_x || cursor_state.y != old_y) {
        if (cursor_state.x != prev.x || cursor_state.y != prev.y) {
            menu_control_mode = 0;
        }
    }
    prev.x = cursor_state.x;
    prev.y = cursor_state.y;
    
    unsigned char nav_pressed = buttons.nav & 0x1E;
    if (nav_pressed && !prev.nav) menu_control_mode = 1;
    prev.nav = nav_pressed;
    
    // Hover detection
    unsigned char cursor_hover = 0xFF;
    if (menu_control_mode == 0) {
        for (unsigned char i = 0; i < 4; i++) {
            if ((current_menu->scroll_offset + i) < current_menu->total_items) {
                if (is_cursor_over_item(i)) {
                    cursor_hover = current_menu->selected_item = i;
                    break;
                }
            }
        }
    }
    
    // Draw highlight
    if (menu_control_mode == 1 || cursor_hover != 0xFF) {
        draw_rectangle(8, 1 + (current_menu->selected_item << 4), 112, 12);
    }
    
    // Draw items
    MenuItem temp_item;
    for (unsigned char i = 0; i < 4; i++) {
        unsigned char idx = current_menu->scroll_offset + i;
        if (idx < current_menu->total_items) {
            read_menu_item(&current_menu->items[idx], &temp_item);
            draw_menu_item(12, 3 + (i << 4), temp_item.label, temp_item.icon_tile);
        }
    }
    
    // Scrollbar (simplified)
    draw_line(126, 0, 64, 1, 1);
    unsigned char bar_h = 5;
    unsigned char bar_y = (current_menu->total_items > 4) ? 
        (current_menu->scroll_offset * (64 - bar_h)) / (current_menu->total_items - 4) : 0;
    for (unsigned char x = 125; x <= 127; x++) {
        draw_line(x, bar_y, bar_h, 1, 0);
    }
    
    // Input handling
    static unsigned char scroll_counter = 0;
    
    if (Flag_screen) {
        scroll_counter++;
        
        // Scrolling
        if (menu_control_mode == 1) {
            if (buttons.NU && !prev.NU) menu_scroll_up();
            if (buttons.ND && !prev.ND) menu_scroll_down();
            prev.NU = buttons.NU;
            prev.ND = buttons.ND;
        } else if (scroll_counter >= 5) {
            if (joystick_dir == DOWN) menu_scroll_down();
            else if (joystick_dir == UP) menu_scroll_up();
            scroll_counter = 0;
        }
        
        // Selection
        unsigned char select = (menu_control_mode == 1 && ((buttons.NB && !prev.NB) || (buttons.R6 && !prev.R6))) ||
                              (menu_control_mode == 0 && cursor_hover != 0xFF && cursor_state.clicking && !prev.cursor_click);
        
        if (select) {
            unsigned char abs_idx = current_menu->scroll_offset + 
                                   (menu_control_mode ? current_menu->selected_item : cursor_hover);
            if (abs_idx < current_menu->total_items) {
                read_menu_item(&current_menu->items[abs_idx], &temp_item);
                if (temp_item.submenu) enter_submenu(temp_item.submenu);
                else if (temp_item.action) temp_item.action();
            }
        }
        
        prev.NB = buttons.NB;
        prev.R6 = buttons.R6;
        prev.cursor_click = cursor_state.clicking;
        
        // Back button
        if (buttons.R5 && !prev.R5 && current_menu->parent_menu) {
            go_back_to_parent();
        }
        prev.R5 = buttons.R5;
    }
    
    if (menu_control_mode == 0) draw_menu_cursor();
}