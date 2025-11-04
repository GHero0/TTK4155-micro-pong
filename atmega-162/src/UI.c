#include "UI.h"
#include "sprites.h"
#include "images.h"
#include "drivers/IO_board.h"
#include "joystick.h"
#include "menu.h"

#include <stdio.h>
#include <avr/pgmspace.h>
#include <string.h>


ScreenState current_screen = SCREEN_MENU;
CursorState cursor_state = {0, 0, 0};
char X_window_1 = 0;
char Y_window_1 = 0;
char X_window_2 = 0;
char Y_window_2 = 0;

void draw_window(int X, int Y, unsigned char width_in_tiles, unsigned char height_in_tiles)
{
    unsigned char width_pixels = width_in_tiles << 3;
    
    // Draw window controls (X, Y, - buttons) in top-right
    fetch_tile_from_tilemap_1bpp(8);                    // X to close
    draw_tile_1bpp(X + width_pixels - 8, Y);
    
    fetch_tile_from_tilemap_1bpp(7);                    // Y to maximize
    draw_tile_1bpp(X + width_pixels - 16, Y);
    
    fetch_tile_from_tilemap_1bpp(6);                    // - to minimize
    draw_tile_1bpp(X + width_pixels - 24, Y);
    
    // Draw title bar background (black tiles)
    fetch_tile_from_tilemap_1bpp(59); // Black background
    for (unsigned char col = 0; col < width_in_tiles - 3; col++)
    {
        draw_tile_1bpp(X + (col << 3), Y);
    }
    
    // Draw window content area background (black tiles)
    for (unsigned char row = 0; row < height_in_tiles; row++)
    {
        fetch_tile_from_tilemap_1bpp(59); // Black background
        for (unsigned char col = 0; col < width_in_tiles; col++)
        {
            draw_tile_1bpp(X + (col << 3), Y + 8 + (row << 3));
        }
    }
    
    // Draw title bar separator line (horizontal line below title bar)
    draw_line(X, Y + 8, width_pixels, 0,0);
    
    // Draw window border rectangle
    draw_rectangle(X, Y, width_in_tiles, height_in_tiles + 1);
}


void joystick_indicator(char X, char Y, unsigned char hand)
{
    if (hand == 0)
    { // Left hand joystick
        switch (joystick_dir)
        {
        case UP:
            fetch_tile_from_tilemap_2bpp(1);
            draw_tile_2bpp(X, Y - 1);
            fetch_tile_from_tilemap_2bpp(2);
            draw_tile_2bpp(X, Y - 8);
            break;
        case DOWN:
            fetch_tile_from_tilemap_2bpp(1);
            draw_tile_2bpp(X, Y + 1);
            fetch_tile_from_tilemap_2bpp(2);
            SYM_V_2bpp();
            draw_tile_2bpp(X, Y + 8);
            break;
        case LEFT:
            fetch_tile_from_tilemap_2bpp(1);
            draw_tile_2bpp(X - 1, Y);
            fetch_tile_from_tilemap_2bpp(3);
            SYM_H_2bpp();
            draw_tile_2bpp(X - 8, Y);
            break;
        case RIGHT:
            fetch_tile_from_tilemap_2bpp(1);
            draw_tile_2bpp(X + 1, Y);
            fetch_tile_from_tilemap_2bpp(3);
            draw_tile_2bpp(X + 8, Y);
            break;
        default:
            fetch_tile_from_tilemap_2bpp(1);
            draw_tile_2bpp(X, Y);
            break;
        }
    }
    else
    { // Right hand joystick = "nav"
        switch (buttons.nav)
        {
        case 0x10: // UP
            fetch_tile_from_tilemap_2bpp(0);
            draw_tile_2bpp(X, Y - 1);
            fetch_tile_from_tilemap_2bpp(2);
            draw_tile_2bpp(X, Y - 8);
            break;

        case 0x04: // DOWN
            fetch_tile_from_tilemap_2bpp(0);
            draw_tile_2bpp(X, Y + 1);
            fetch_tile_from_tilemap_2bpp(2);
            SYM_V_2bpp();
            draw_tile_2bpp(X, Y + 8);
            break;

        case 0x08: // LEFT
            fetch_tile_from_tilemap_2bpp(0);
            draw_tile_2bpp(X - 1, Y);
            fetch_tile_from_tilemap_2bpp(3);
            SYM_H_2bpp();
            draw_tile_2bpp(X - 8, Y);
            break;

        case 0x02: // RIGHT
            fetch_tile_from_tilemap_2bpp(0);
            draw_tile_2bpp(X + 1, Y);
            fetch_tile_from_tilemap_2bpp(3);
            draw_tile_2bpp(X + 8, Y);
            break;

        case 0x06: // DOWN + RIGHT
            fetch_tile_from_tilemap_2bpp(0);
            draw_tile_2bpp(X + 1, Y + 1);
            fetch_tile_from_tilemap_2bpp(4);
            SYM_V_2bpp();
            draw_tile_2bpp(X + 7, Y + 7);
            break;

        case 0x0C: // DOWN + LEFT
            fetch_tile_from_tilemap_2bpp(0);
            draw_tile_2bpp(X - 1, Y + 1);
            fetch_tile_from_tilemap_2bpp(4);
            SYM_V_2bpp();
            SYM_H_2bpp();
            draw_tile_2bpp(X - 7, Y + 7);
            break;

        case 0x18: // Up + LEFT
            fetch_tile_from_tilemap_2bpp(0);
            draw_tile_2bpp(X - 1, Y - 1);
            fetch_tile_from_tilemap_2bpp(4);
            SYM_H_2bpp();
            draw_tile_2bpp(X - 7, Y - 7);
            break;

        case 0x12: // Up + RIGHT
            fetch_tile_from_tilemap_2bpp(0);
            draw_tile_2bpp(X + 1, Y - 1);
            fetch_tile_from_tilemap_2bpp(4);
            draw_tile_2bpp(X + 7, Y - 7);
            break;

        default:
            fetch_tile_from_tilemap_2bpp(0);
            draw_tile_2bpp(X, Y);
            break;
        }
    }
}

void button_indicator(char X, char Y, unsigned char hand, unsigned char number)
{
    char x = X;
    char y = Y;
    unsigned char tile_base;
    uint8_t pressed = 0;

    // --- Determine which hand and button pressed ---
    if (hand == 0)
    {
        // Left hand: 7 buttons (bits 0–6)
        tile_base = 16;
        if (number >= 1 && number <= 7)
        {
            pressed = (buttons.left >> (number - 1)) & 0x01;
        }
    }
    else
    {
        // Right hand: 6 buttons (bits 0–5)
        tile_base = 32;
        if (number >= 1 && number <= 6)
        {
            pressed = (buttons.right >> (number - 1)) & 0x01;
        }
    }

    // --- Shift all tiles by +1 pixel if the corresponding button is pressed ---
    if (pressed)
    {
        y++;
        x++;
    }
    else
    {
        draw_rectangle(x + 1, y + 1, 3, 1);
    }

    // --- Draw background frame ---

    fetch_tile_from_tilemap_1bpp(59);
    draw_tile_1bpp(x, y);
    draw_tile_1bpp(x + 8, y);
    draw_tile_1bpp(x + 16, y);
    draw_rectangle(x, y, 3, 1);

    // --- Draw 'B' for button ---
    fetch_tile_from_tilemap_2bpp(16);
    draw_tile_2bpp(x + 6, y);

    // --- Draw hand indicator (L or R) ---
    fetch_tile_from_tilemap_2bpp(tile_base);
    draw_tile_2bpp(x, y);

    // --- Select number tile ---
    unsigned char tile_id = 0xFF;

    switch (number)
    {
    case 1:
        tile_id = 17;
        break;
    case 2:
        tile_id = 17 + 16;
        break;
    case 3:
        tile_id = 17 + 32;
        break;
    case 4:
        tile_id = 18;
        break;
    case 5:
        tile_id = 18 + 16;
        break;
    case 6:
        tile_id = 18 + 32;
        break;
    case 7:
        tile_id = 19;
        break; // Left hand only
    default:
        break;
    }

    // --- Draw number tile if valid ---
    if (tile_id != 0xFF)
    {
        fetch_tile_from_tilemap_2bpp(tile_id);
        draw_tile_2bpp(x + 16, y);
    }
}

void draw_printf(char x, char y, const char* fmt, ...) {
    char buf[32]; // adjust as needed (keep small on AVR)
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    draw_string(buf, x, y);
}

char X = 30;
char Y = 30;

void debug_window(void)
{
    unsigned char Y_window_1 = 8;
    unsigned char Y_window_2 = 16;
    unsigned char X_window_1 = 8;
    unsigned char X_window_2 = 72;

    if (Y > 0) {
        draw_line(127, 0, Y, 1, 1);  // vertical from (127, 0) down Y pixels
    }
    if (X < 127) {
        draw_line(X, Y, 127 - X, 0, 2);  // horizontal from (X, Y) to (127, Y)
    }

    draw_window(X_window_2, Y_window_2, 6, 3);
    draw_window(X_window_1, Y_window_1, 7, 4);

    draw_printf(X_window_1 + 3, Y_window_1 + 16, "X:%d\nY:%d", joystick_pos.X >> 8, joystick_pos.Y >> 8);
    draw_printf(X_window_1 + 27, Y_window_1 + 16, "X':%d\nY':%d\nS:%d", X, Y, touch_pad.size);

    joystick_indicator(X_window_2 + 8, Y_window_2 + 16, 0);
    joystick_indicator(X_window_2 + 32, Y_window_2 + 16, 1);

    update_cursor_position();
    draw_menu_cursor();
}

void map_touchpad(void)
{
    // Map touchpad
    X = touch_pad.x >> 1;
    Y = 63 - (touch_pad.y >> 2);

    if (X < 0)
        X = 0;
    if (X > 127)
        X = 127;
    if (Y < 0)
        Y = 0;
    if (Y > 63)
        Y = 63;
}

void display_current_screen(void) {
    // Initialize menus on first call
    
    switch (current_screen) {
        case SCREEN_MENU:
            if (current_menu == NULL) {
                current_menu = &main_menu;
            }
            draw_menu();
            break;
            
        case SCREEN_DEBUG:
            debug_window();
            static unsigned char prev_back_button = 0;
            if (buttons.R5 && !prev_back_button) {
                current_screen = SCREEN_MENU;
            }
            prev_back_button = buttons.R5;
            break;
            
        default:
            current_screen = SCREEN_MENU;
            break;
    }
}