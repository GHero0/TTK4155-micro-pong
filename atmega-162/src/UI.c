#include "UI.h"
#include "sprites.h"
#include "images.h"
#include "drivers/IO_board.h"
#include "joystick.h"
#include <stdio.h>

void draw_window(int X, int Y, unsigned char width_in_tiles, unsigned char height_in_tiles)
{

    // Similar to Windows Title Bar
    fetch_tile_from_tilemap_1bpp(8);                   // X to close
    draw_tile_1bpp(X + (width_in_tiles << 3) - 8, Y);  // Top-right corner
    fetch_tile_from_tilemap_1bpp(7);                   // Y to maximize
    draw_tile_1bpp(X + (width_in_tiles << 3) - 16, Y); // Top-right corner
    fetch_tile_from_tilemap_1bpp(6);                   // - to minimize
    draw_tile_1bpp(X + (width_in_tiles << 3) - 24, Y); // Top-right corner
    // Other tiles, draw title bar as black tiles
    fetch_tile_from_tilemap_1bpp(59); // Black background
    for (unsigned char col = 0; col < width_in_tiles - 3; col++)
    {
        draw_tile_1bpp(X + (col << 3), Y);
    }

    for (unsigned char row = 0; row < height_in_tiles; row++)
    {
        fetch_tile_from_tilemap_1bpp(59); // Black background
        for (unsigned char col = 0; col < width_in_tiles; col++)
        {
            draw_tile_1bpp(X + (col << 3), Y + 8 + (row << 3));
        }
    }

    // Draw rectangle border
    draw_line(X, Y + 8, X + (width_in_tiles << 3) - 1, Y + 8); // title header
    // title buttons barrier
    draw_rectangle(X, Y, width_in_tiles, height_in_tiles + 1);
}

void draw_task_bar(void)
{
    unsigned char Y = 56;

    fetch_tile_from_tilemap_1bpp(59); // Black background
    for (unsigned char col = 0; col < 14; col++)
    {
        draw_tile_1bpp(col << 3, Y);
    }

    fetch_tile_from_tilemap_1bpp(10);
    draw_tile_1bpp(2, Y + 1); // Start button
    fetch_tile_from_tilemap_1bpp(9);
    draw_tile_1bpp(8 + 2, Y + 1); // Search button

    fetch_tile_from_tilemap_1bpp(6);
    draw_tile_1bpp(15, Y); // Task view button

    // Draw fancy dithering background
    fetch_tile_from_tilemap_1bpp(4); // Dark gray tile
    SYM_H_1bpp();
    draw_tile_1bpp((15 << 3), Y);
    fetch_tile_from_tilemap_1bpp(5); // Dark gray tile
    SYM_H_1bpp();
    draw_tile_1bpp((14 << 3), Y);

    // Draw rectangle border
    draw_line(0, Y - 1, 127, Y - 1); // Top border
    draw_line(0, 63, 127, 63);       // Bottom border
    draw_line(0, Y - 1, 0, 63);      // Left
    draw_line(127, Y - 1, 127, 63);  // Right
    //
    draw_line(8, Y, 8, 63);
    draw_line(24, Y, 24, 63);
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
// * Would need to be changed at some point
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

void cursor()
{
    static int x = 8; // Use int to avoid overflow issues
    static int y = 8;

    signed char dx = joystick_pos.X >> 8; // [-100, 100]
    signed char dy = joystick_pos.Y >> 8;

    // Deadzone threshold
    const int deadzone = 3;

    // Move cursor if joystick input exceeds deadzone
    if (dx > deadzone || dx < -deadzone)
        x += dx / 32;  // Adjust sensitivity by divisor
    if (dy > deadzone || dy < -deadzone)
        y -= dy / 32;  // Invert Y axis

    // Clamp cursor position to screen bounds
    if (x < -4) x = -4;
    if (x > 124) x = 124; // 128 - 8 tile width
    if (y < -4) y = -4;
    if (y > 60) y = 60;   // 64 - 8 tile height

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

    // Choose base tile and draw without complicated offset logic
    unsigned char base_tile = 6 + tile_index;

    fetch_tile_from_tilemap_2bpp(base_tile);

    draw_tile_2bpp((unsigned char)x, (unsigned char)y);
}

void draw_printf(char x, char y, const char* fmt, ...) {
    char buf[32]; // adjust as needed (keep small on AVR)
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    draw_string(buf, x, y);
}