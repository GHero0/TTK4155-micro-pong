#ifndef UI_H
#define UI_H

// =============================================================================
// INCLUDES
// =============================================================================

// Personal headers
#include "types.h"

// =============================================================================
// GENERIC UI COMPONENTS
// =============================================================================

void draw_window(int X, int Y, unsigned char width_in_tiles, unsigned char height_in_tiles);
void joystick_indicator(char X, char Y, unsigned char hand);
void button_indicator(char X, char Y, unsigned char hand, unsigned char number);
void draw_printf(char x, char y, const char* fmt, ...);
void draw_menu_item(unsigned char x, unsigned char y, const char* label, unsigned char icon_tile);
void Shade_Background(void);

// =============================================================================
// CURSOR FUNCTIONS
// =============================================================================

void update_cursor_position(void);
void draw_menu_cursor(void);

// =============================================================================
// SCREEN RENDERING FUNCTIONS
// =============================================================================

void debug_io_board(void);
void debug_blue_box(void);
void calibration_joystick(void);
void startup(void);

// =============================================================================
// SCREEN MANAGER
// =============================================================================

void display_current_screen(void);

// =============================================================================
// GLOBAL STATE
// =============================================================================

extern ScreenState current_screen;
extern CursorState cursor_state;
extern unsigned char prev_R4;
extern unsigned char score;

#endif