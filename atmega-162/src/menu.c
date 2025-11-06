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
    {"Submenu 1-1", NULL, NULL, 30},
    {"Submenu 1-2", NULL, NULL, 30},
    {"Submenu 1-3", NULL, NULL, 30},
};

const MenuItem PROGMEM submenu_2_items[] = {
    {"..", go_back_to_parent, NULL, 31},
    {"Submenu 2-1", NULL, NULL, 30},
    {"Submenu 2-2", NULL, NULL, 30},
};

const MenuItem PROGMEM main_menu_items[] = {
    {"Debug IO-board", action_debug_window, NULL, 15},
    {"Settings", NULL, &submenu_1, 30},
    {"Options", NULL, &submenu_2, 30},
    {"Debug Blue Box", action_placeholder_4, NULL, 46},
    {"Menu Item 5", NULL, NULL, 255},
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
    const signed char cy = cursor_state.y + 4; // Center of the drawn cusror
    
    return (cx >= item_x && cx < item_x + item_width &&
            cy >= item_y && cy < item_y + item_height);
}

// Helper function to go back to parent menu
void go_back_to_parent(void) {
    if (current_menu != NULL && current_menu->parent_menu != NULL) {
        current_menu = current_menu->parent_menu;
    }
}

// Menu action functions - NOW WITH TRANSITIONS!
void action_debug_window(void) {
    Transition_Start(SCREEN_DEBUG_IO_BOARD);
}

void action_placeholder_2(void) {
    // Action for menu item 2
}

void action_placeholder_3(void) {
    // Action for menu item 3
}

void action_placeholder_4(void) {
    Transition_Start(SCREEN_DEBUG_BLUE_BOX);
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

void draw_menu(void) {
    if (current_menu == NULL) return;
    
    // Detect input mode switching
    static signed char last_cursor_x = 0;
    static signed char last_cursor_y = 0;
    static unsigned char prev_nav_for_mode = 0;
    
    // Always update cursor position to detect movement
    signed char old_x = cursor_state.x;
    signed char old_y = cursor_state.y;
    update_cursor_position();
    
    // Check if cursor actually moved (switch to cursor mode)
    if (cursor_state.x != old_x || cursor_state.y != old_y) {
        if (cursor_state.x != last_cursor_x || cursor_state.y != last_cursor_y) {
            menu_control_mode = 0; // Switch to cursor mode
        }
    }
    last_cursor_x = cursor_state.x;
    last_cursor_y = cursor_state.y;
    
    // Check if nav buttons pressed (switch to nav mode)
    unsigned char nav_pressed = buttons.nav & 0x1E; // NU, NL, ND, NR (bits 1-4)
    if (nav_pressed && !prev_nav_for_mode) {
        menu_control_mode = 1; // Switch to nav button mode
    }
    prev_nav_for_mode = nav_pressed;
    
    // --- HOVER DETECTION FIXED ---
    unsigned char cursor_hover_item = 0xFF; // no hover by default

    if (menu_control_mode == 0) {
        for (unsigned char i = 0; i < 4; i++) {
            unsigned char item_index = current_menu->scroll_offset + i;
            if (item_index < current_menu->total_items) {
                if (is_cursor_over_item(i)) {
                    cursor_hover_item = i;
                    break; // stop after first match
                }
            }
        }

        // Only update selected_item when actually hovering something
        if (cursor_hover_item != 0xFF) {
            current_menu->selected_item = cursor_hover_item;
        }
    }

    // --- DRAW HIGHLIGHT (only if needed) ---
    if (menu_control_mode == 1 || cursor_hover_item != 0xFF) {
        signed char base_y = 1 + (current_menu->selected_item * 16);

        // TOP border of menu item selected
        draw_line(8, base_y, 112, 0, 0);
        // BOTTOM border (double line)
        draw_line(8, base_y + 11, 112, 0, 0);
        draw_line(8, base_y + 12, 112, 0, 0);
        // LEFT Border
        draw_line(7, base_y + 1, 11, 1, 0);
        // RIGHT Border (double line)
        draw_line(120, base_y + 1, 11, 1, 0);
        draw_line(121, base_y + 1, 11, 1, 0);
    }

    // --- DRAW MENU TEXT AND ICONS ---
    MenuItem temp_item;
    for (unsigned char i = 0; i < 4; i++) {
        unsigned char item_index = current_menu->scroll_offset + i;
        if (item_index < current_menu->total_items) {
            read_menu_item(&current_menu->items[item_index], &temp_item);
            
            // Check if icon exists (255 = no icon)
            if (temp_item.icon_tile != 255) {
                // Draw icon (8x8 tile at left side of menu item)
                fetch_tile_from_tilemap_1bpp(temp_item.icon_tile);
                draw_tile_1bpp(12, 3 + (i * 16));
                
                // Draw text (shifted right to make room for icon)
                if (temp_item.submenu != NULL) {
                    draw_printf(22, 3 + (i * 16), "%s >", temp_item.label);
                } else {
                    draw_printf(22, 3 + (i * 16), temp_item.label);
                }
            } else {
                // No icon - draw text at original position
                if (temp_item.submenu != NULL) {
                    draw_printf(12, 3 + (i * 16), "%s >", temp_item.label);
                } else {
                    draw_printf(12, 3 + (i * 16), temp_item.label);
                }
            }
        }
    }

    // --- SCROLLBAR ---
    draw_line(126, 0, 64, 1, 1); // scrollbar track (dotted line)
    
    unsigned char scrollbar_height = 5;
    unsigned char track_height = 64 - scrollbar_height;
    
    if (current_menu->total_items > 4) {
        unsigned char max_scroll = current_menu->total_items - 4;
        unsigned char scroll_y = (current_menu->scroll_offset * track_height) / max_scroll;
        
        draw_line(125, scroll_y, scrollbar_height, 1, 0);
        draw_line(126, scroll_y, scrollbar_height, 1, 0);
        draw_line(127, scroll_y, scrollbar_height, 1, 0);
    } else {
        draw_line(125, 0, scrollbar_height, 1, 0);
        draw_line(126, 0, scrollbar_height, 1, 0);
        draw_line(127, 0, scrollbar_height, 1, 0);
    }

    // --- INPUT HANDLING (Skip during transitions) ---
    if (Transition_Is_Active()) {
        // Don't process input during transitions
        if (menu_control_mode == 0) {
            draw_menu_cursor();
        }
        return;
    }
    
    static unsigned char scroll_counter = 0;
    static unsigned char prev_R6_state = 0;
    static unsigned char prev_R5_state = 0;
    static unsigned char prev_cursor_click = 0;
    static unsigned char prev_NU_state = 0;
    static unsigned char prev_ND_state = 0;
    static unsigned char prev_NB_state = 0;

    if (Flag_screen) {
        scroll_counter++;
        
        // Nav button mode navigation
        if (menu_control_mode == 1) {
            if ((buttons.NU) && !prev_NU_state) menu_scroll_up();
            if ((buttons.ND) && !prev_ND_state) menu_scroll_down();
            prev_NU_state = buttons.NU;
            prev_ND_state = buttons.ND;
        }
        // Cursor mode navigation (slow scroll)
        else {
            if (scroll_counter >= 5) {
                if (joystick_dir == DOWN) menu_scroll_down();
                else if (joystick_dir == UP) menu_scroll_up();
                scroll_counter = 0;
            }
        }
        
        // R6 button press (click)
        if ((buttons.R6) && !prev_R6_state && cursor_hover_item == 0xFF) {
            unsigned char absolute_index = current_menu->scroll_offset + current_menu->selected_item;
            if (absolute_index < current_menu->total_items) {
                read_menu_item(&current_menu->items[absolute_index], &temp_item);
                if (temp_item.submenu != NULL) enter_submenu(temp_item.submenu);
                else if (temp_item.action != NULL) temp_item.action();
            }
        }
        prev_R6_state = buttons.R6;
        
        // NB button press (select in nav mode)
        if ((buttons.NB) && !prev_NB_state && menu_control_mode == 1) {
            unsigned char absolute_index = current_menu->scroll_offset + current_menu->selected_item;
            if (absolute_index < current_menu->total_items) {
                read_menu_item(&current_menu->items[absolute_index], &temp_item);
                if (temp_item.submenu != NULL) enter_submenu(temp_item.submenu);
                else if (temp_item.action != NULL) temp_item.action();
            }
        }
        prev_NB_state = buttons.NB;
        
        // Cursor click (only in cursor mode)
        if (menu_control_mode == 0 && cursor_hover_item != 0xFF && cursor_state.clicking && !prev_cursor_click) {
            unsigned char absolute_index = current_menu->scroll_offset + cursor_hover_item;
            if (absolute_index < current_menu->total_items) {
                read_menu_item(&current_menu->items[absolute_index], &temp_item);
                if (temp_item.submenu != NULL) enter_submenu(temp_item.submenu);
                else if (temp_item.action != NULL) temp_item.action();
            }
        }
        prev_cursor_click = cursor_state.clicking;
        
        // R5 button press (back)
        if ((buttons.R5) && !prev_R5_state) {
            if (current_menu->parent_menu != NULL) {
                go_back_to_parent();
            }
        }
        prev_R5_state = buttons.R5;
    }

    // --- DRAW CURSOR ---
    if (menu_control_mode == 0) {
        draw_menu_cursor();
    }
}