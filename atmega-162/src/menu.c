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

// Sub-menu items
MenuItem submenu_1_items[] = {
    {"..", go_back_to_parent, NULL},
    {"Submenu 1-1", NULL, NULL},
    {"Submenu 1-2", NULL, NULL},
    {"Submenu 1-3", NULL, NULL},
};

MenuItem submenu_2_items[] = {
    {"..", go_back_to_parent, NULL},
    {"Submenu 2-1", NULL, NULL},
    {"Submenu 2-2", NULL, NULL},
};

Menu submenu_1 = {
    .selected_item = 0,
    .total_items = 4,
    .scroll_offset = 0,
    .visible_items = 4,
    .items = submenu_1_items,
    .parent_menu = NULL,
    .has_back_option = 1
};

Menu submenu_2 = {
    .selected_item = 0,
    .total_items = 3,
    .scroll_offset = 0,
    .visible_items = 4,
    .items = submenu_2_items,
    .parent_menu = NULL,
    .has_back_option = 1
};

MenuItem main_menu_items[] = {
    {"Debug Window", action_debug_window, NULL},
    {"Settings", NULL, &submenu_1},
    {"Options", NULL, &submenu_2},
    {"Menu Item 4", action_placeholder_4, NULL},
    {"Menu Item 5", NULL, NULL},
    {"Menu Item 6", NULL, NULL},
    {"Menu Item 7", NULL, NULL},
    {"Menu Item 8", NULL, NULL},
    {"Menu Item 9", NULL, NULL},
    {"Menu Item 10", NULL, NULL}
};

Menu main_menu = {
    .selected_item = 0,
    .total_items = 10,
    .scroll_offset = 0,
    .visible_items = 4,
    .items = main_menu_items,
    .parent_menu = NULL,
    .has_back_option = 0
};

// Store menu items in flash memory
const MenuItem PROGMEM submenu_1_items_flash[4] = {
    {"../", go_back_to_parent, NULL},
    {"Submenu 1-1", NULL, NULL},
    {"Submenu 1-2", NULL, NULL},
    {"Submenu 1-3", NULL, NULL},
};

const MenuItem PROGMEM submenu_2_items_flash[3] = {
    {"../", go_back_to_parent, NULL},
    {"Submenu 2-1", NULL, NULL},
    {"Submenu 2-2", NULL, NULL},
};

const MenuItem PROGMEM main_menu_items_flash[10] = {
    {"Debug Window", action_debug_window, NULL},
    {"Settings", NULL, &submenu_1},
    {"Options", NULL, &submenu_2},
    {"Menu Item 4", action_placeholder_4, NULL},
    {"Menu Item 5", NULL, NULL},
    {"Menu Item 6", NULL, NULL},
    {"Menu Item 7", NULL, NULL},
    {"Menu Item 8", NULL, NULL},
    {"Menu Item 9", NULL, NULL},
    {"Menu Item 10", NULL, NULL}
};

// Simple copy from flash to RAM
void init_menus(void) {
    static unsigned char initialized = 0;
    if (initialized) return;
    
    memcpy_P(submenu_1_items, submenu_1_items_flash, sizeof(submenu_1_items_flash));
    memcpy_P(submenu_2_items, submenu_2_items_flash, sizeof(submenu_2_items_flash));
    memcpy_P(main_menu_items, main_menu_items_flash, sizeof(main_menu_items_flash));
    
    initialized = 1;
}

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

// Helper function to check if cursor is hovering over a menu item
unsigned char is_cursor_over_item(unsigned char item_visual_index) {
    signed char item_x = 8;
    signed char item_y = 1 + (item_visual_index * 16);
    signed char item_width = 112;
    signed char item_height = 14;
    
    return (cursor_state.x + 8 > item_x && 
            cursor_state.x < item_x + item_width &&
            cursor_state.y + 8 > item_y && 
            cursor_state.y < item_y + item_height);
}

// Helper function to go back to parent menu
void go_back_to_parent(void) {
    if (current_menu != NULL && current_menu->parent_menu != NULL) {
        current_menu = current_menu->parent_menu;
    }
}

// Menu action functions
void action_debug_window(void) {
    current_screen = SCREEN_DEBUG;
}

void action_placeholder_2(void) {
    // Action for menu item 2
}

void action_placeholder_3(void) {
    // Action for menu item 3
}

void action_placeholder_4(void) {
    // Action for menu item 4
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
        if (current_menu->selected_item < current_menu->visible_items - 1) {
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
    
    // Check if cursor is hovering over any visible item (only in cursor mode)
    unsigned char cursor_hover_item = 0xFF; // 0xFF means no hover
    if (menu_control_mode == 0) {
        for (unsigned char i = 0; i < current_menu->visible_items; i++) {
            unsigned char item_index = current_menu->scroll_offset + i;
            if (item_index < current_menu->total_items) {
                if (is_cursor_over_item(i)) {
                    cursor_hover_item = i;
                    current_menu->selected_item = i;
                    break;
                }
            }
        }
    }
    
    // Calculate Y position for selection highlight
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
    
    // Draw visible menu items (4 on screen)
    for (unsigned char i = 0; i < current_menu->visible_items; i++) {
        unsigned char item_index = current_menu->scroll_offset + i;
        if (item_index < current_menu->total_items) {
            if (current_menu->items[item_index].submenu != NULL) {
                draw_printf(12, 3 + (i * 16), "%s >", current_menu->items[item_index].label);
            } else {
                draw_printf(12, 3 + (i * 16), current_menu->items[item_index].label);
            }
        }
    }
    
    // Scrollbar background (dotted line)
    draw_line(126, 0, 64, 1, 1);
    
    // Calculate scrollbar position
    unsigned char scrollbar_height = 5;
    unsigned char track_height = 64 - scrollbar_height;
    
    if (current_menu->total_items > current_menu->visible_items) {
        unsigned char max_scroll = current_menu->total_items - current_menu->visible_items;
        unsigned char scroll_y = (current_menu->scroll_offset * track_height) / max_scroll;
        
        draw_line(125, scroll_y, scrollbar_height, 1, 0);
        draw_line(126, scroll_y, scrollbar_height, 1, 0);
        draw_line(127, scroll_y, scrollbar_height, 1, 0);
    } else {
        draw_line(125, 0, scrollbar_height, 1, 0);
        draw_line(126, 0, scrollbar_height, 1, 0);
        draw_line(127, 0, scrollbar_height, 1, 0);
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
            if ((buttons.NU) && !prev_NU_state) {
                menu_scroll_up();
            }
            if ((buttons.ND) && !prev_ND_state) {
                menu_scroll_down();
            }
            prev_NU_state = buttons.NU;
            prev_ND_state = buttons.ND;
        }
        // Cursor mode navigation (slower, every 5 frames)
        else {
            if (scroll_counter >= 5) {
                if (joystick_dir == DOWN) {
                    menu_scroll_down();
                } else if (joystick_dir == UP) {
                    menu_scroll_up();
                }
                scroll_counter = 0;
            }
        }
        
        // Check for R6 button press (works in both modes)
        if ((buttons.R6) && !prev_R6_state && cursor_hover_item == 0xFF) {
            unsigned char absolute_index = current_menu->scroll_offset + current_menu->selected_item;
            if (absolute_index < current_menu->total_items) {
                MenuItem* selected = &current_menu->items[absolute_index];
                
                if (selected->submenu != NULL) {
                    enter_submenu(selected->submenu);
                } else if (selected->action != NULL) {
                    selected->action();
                }
            }
        }
        prev_R6_state = buttons.R6;
        
        // Check for NB button press (SELECT in nav mode)
        if ((buttons.NB) && !prev_NB_state && menu_control_mode == 1) {
            unsigned char absolute_index = current_menu->scroll_offset + current_menu->selected_item;
            if (absolute_index < current_menu->total_items) {
                MenuItem* selected = &current_menu->items[absolute_index];
                
                if (selected->submenu != NULL) {
                    enter_submenu(selected->submenu);
                } else if (selected->action != NULL) {
                    selected->action();
                }
            }
        }
        prev_NB_state = buttons.NB;
        
        // Cursor click (only in cursor mode)
        if (menu_control_mode == 0 && cursor_hover_item != 0xFF && cursor_state.clicking && !prev_cursor_click) {
            unsigned char absolute_index = current_menu->scroll_offset + cursor_hover_item;
            if (absolute_index < current_menu->total_items) {
                MenuItem* selected = &current_menu->items[absolute_index];
                
                if (selected->submenu != NULL) {
                    enter_submenu(selected->submenu);
                } else if (selected->action != NULL) {
                    selected->action();
                }
            }
        }
        prev_cursor_click = cursor_state.clicking;
        
        // Check for R5 button press (BACK)
        if ((buttons.R5) && !prev_R5_state) {
            if (current_menu->parent_menu != NULL) {
                go_back_to_parent();
            }
        }
        prev_R5_state = buttons.R5;
    }
    
    // Draw cursor only in cursor mode
    if (menu_control_mode == 0) {
        draw_menu_cursor();
    }
}