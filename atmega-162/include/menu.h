#ifndef MENU_H
#define MENU_H

#include "types.h"

void update_cursor_position(void);
void draw_menu_cursor(void);
void enter_submenu(Menu* submenu);
void go_back_to_parent(void);
void menu_scroll_up(void);
void menu_scroll_down(void);
unsigned char is_cursor_over_item(unsigned char item_visual_index);

// Action functions
void action_debug_window(void);
void action_placeholder_2(void);
void action_placeholder_3(void);
void action_placeholder_4(void);

extern Menu submenu_1;
extern Menu submenu_2;
extern Menu main_menu;

extern Menu * current_menu;

#endif