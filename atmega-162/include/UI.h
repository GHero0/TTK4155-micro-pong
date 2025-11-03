#ifndef UI_H
#define UI_H

#include "types.h"

void draw_window(int X, int Y, unsigned char width_in_tiles, unsigned char height_in_tiles);
void joystick_indicator(char X, char Y, unsigned char hand);
void button_indicator(char X, char Y, unsigned char hand, unsigned char number);
void cursor();
void draw_printf(char x, char y, const char* fmt, ...);

// Menu system functions
void init_menus(void);
void draw_menu(void);
void draw_menu_cursor(void);
void display_current_screen(void);

// Menu system
extern ScreenState current_screen;
extern CursorState cursor_state;

#endif