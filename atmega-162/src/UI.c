/**
 * @file UI.c
 * @brief UI components, cursor system, and screen management
 */

 // =============================================================================
// INCLUDES
// =============================================================================

// Personal headers
#include "UI.h"
#include "menu.h"
#include "sprites.h"
#include "images.h"
#include "drivers/IO_board.h"
#include "drivers/OLED.h"
#include "drivers/CAN.h"
#include "joystick.h"
#include "inter.h"

// LIbraries
#include <stdio.h>
#include <avr/pgmspace.h>
#include <string.h>

// =============================================================================
// GLOBAL STATE
// =============================================================================

ScreenState current_screen = SCREEN_STARTUP;
CursorState cursor_state = {0, 0, 0};
unsigned char score = 0;
unsigned char prev_R4 = 0;

// =============================================================================
// STATIC STATE
// =============================================================================

static unsigned char debug_io_prompt_dismissed = 0;
static unsigned char debug_io_allow_dismiss = 0;

static CalState cal_state = CAL_CENTER;

// =============================================================================
// GENERIC UI COMPONENTS
// =============================================================================

void Shade_Background(void) 
{
    for (uint16_t i = 0; i < 1024; i++) {
        uint8_t row = i >> 4;
        uint8_t col = i & 0x0F;
        uint8_t col_shift = col << 3;
        
        uint8_t mask = ((row + col_shift) & 1) ? 0xAA : 0x55;
        current_buffer[i] &= mask;
    }
}

void draw_window(int X, int Y, unsigned char width_in_tiles, unsigned char height_in_tiles)
{
    for (unsigned char row = 0; row < height_in_tiles; row++)
    {
        fetch_tile_from_tilemap_1bpp(59);
        for (unsigned char col = 0; col < width_in_tiles; col++)
        {
            draw_tile_1bpp(X + (col << 3), Y + (row << 3));
        }
    }
    
    draw_rectangle(X, Y, width_in_tiles << 3, height_in_tiles << 3);
}

void draw_menu_item(unsigned char x, unsigned char y, const char *label, unsigned char icon_tile)
{
    unsigned char text_x = x;

    // Draw icon if it exists
    if (icon_tile != 255)
    {
        fetch_tile_from_tilemap_1bpp(icon_tile);
        draw_tile_1bpp(x, y);
        text_x = x + 10;
    }

    // Draw text
    draw_printf(text_x, y, "%s", label);
}

void draw_printf(char x, char y, const char* fmt, ...) 
{
    char buf[64];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    draw_string(buf, x, y);
}

void button_indicator(char X, char Y, unsigned char hand, unsigned char number)
{
    char x = X;
    char y = Y;
    unsigned char tile_base;
    uint8_t pressed = 0;
    
    #define TILE_L 16
    #define TILE_R 32
    #define TILE_1 17
    #define TILE_2 33
    #define TILE_3 49
    #define TILE_4 18
    #define TILE_5 34
    #define TILE_6 50
    #define TILE_7 19
    
    if (hand == 0)
    {
        tile_base = TILE_L;
        if (number >= 1 && number <= 7)
        {
            pressed = (buttons.left >> (number - 1)) & 0x01;
        }
    }
    else
    {
        tile_base = TILE_R;
        if (number >= 1 && number <= 6)
        {
            pressed = (buttons.right >> (number - 1)) & 0x01;
        }
    }
    
    if (pressed)
    {
        y++;
        x++;
    }
    else
    {
        draw_rectangle(x + 1, y + 1, 16, 8);
    }
    
    fetch_tile_from_tilemap_1bpp(59);
    draw_tile_1bpp(x, y);
    draw_tile_1bpp(x + 8, y);
    draw_rectangle(x, y, 16, 8);
    
    fetch_tile_from_tilemap_2bpp(tile_base);
    draw_tile_2bpp(x + 1, y);
    
    const unsigned char number_tiles[] = {0xFF, TILE_1, TILE_2, TILE_3, TILE_4, TILE_5, TILE_6, TILE_7};
    unsigned char tile_id = (number >= 1 && number <= 7) ? number_tiles[number] : 0xFF;
    
    if (tile_id != 0xFF)
    {
        fetch_tile_from_tilemap_2bpp(tile_id);
        draw_tile_2bpp(x + 7, y);
    }
}

static void draw_joystick_direction(char X, char Y, JoyDir direction, unsigned char hand)
{
    switch (direction)
    {
        case UP:
            fetch_tile_from_tilemap_2bpp(1-hand);
            draw_tile_2bpp(X, Y - 1);
            fetch_tile_from_tilemap_2bpp(2);
            draw_tile_2bpp(X, Y - 8);
            break;

        case DOWN:
            fetch_tile_from_tilemap_2bpp(1-hand);
            draw_tile_2bpp(X, Y + 1);
            fetch_tile_from_tilemap_2bpp(2);
            SYM_V_2bpp();
            draw_tile_2bpp(X, Y + 8);
            break;

        case LEFT:
            fetch_tile_from_tilemap_2bpp(1-hand);
            draw_tile_2bpp(X - 1, Y);
            fetch_tile_from_tilemap_2bpp(3);
            SYM_H_2bpp();
            draw_tile_2bpp(X - 8, Y);
            break;

        case RIGHT:
            fetch_tile_from_tilemap_2bpp(1-hand);
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

void joystick_indicator(char X, char Y, unsigned char hand)
{
    if (hand == 0)
    {
        draw_joystick_direction(X, Y, joystick_dir, 0);
    }
    else
    {
        switch (buttons.nav)
        {
            case 0x10:
                draw_joystick_direction(X, Y, UP, 1);
                break;
            case 0x04:
                draw_joystick_direction(X, Y, DOWN, 1);
                break;
            case 0x08:
                draw_joystick_direction(X, Y, LEFT, 1);
                break;
            case 0x02:
                draw_joystick_direction(X, Y, RIGHT, 1);
                break;

            case 0x06:
                fetch_tile_from_tilemap_2bpp(0);
                draw_tile_2bpp(X + 1, Y + 1);
                fetch_tile_from_tilemap_2bpp(4);
                SYM_V_2bpp();
                draw_tile_2bpp(X + 7, Y + 7);
                break;

            case 0x0C:
                fetch_tile_from_tilemap_2bpp(0);
                draw_tile_2bpp(X - 1, Y + 1);
                fetch_tile_from_tilemap_2bpp(4);
                SYM_V_2bpp();
                SYM_H_2bpp();
                draw_tile_2bpp(X - 7, Y + 7);
                break;

            case 0x18:
                fetch_tile_from_tilemap_2bpp(0);
                draw_tile_2bpp(X - 1, Y - 1);
                fetch_tile_from_tilemap_2bpp(4);
                SYM_H_2bpp();
                draw_tile_2bpp(X - 7, Y - 7);
                break;

            case 0x12:
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

// =============================================================================
// CURSOR SYSTEM
// =============================================================================

void update_cursor_position(void)
{
    signed char dx = joystick_pos.X >> 8;
    signed char dy = joystick_pos.Y >> 8;

    const int deadzone = 3;

    if (dx > deadzone || dx < -deadzone)
        cursor_state.x += dx / 32;
    if (dy > deadzone || dy < -deadzone)
        cursor_state.y -= dy / 32;

    if (cursor_state.x < -4)
        cursor_state.x = -4;
    if (cursor_state.x > 124)
        cursor_state.x = 124;
    if (cursor_state.y < -4)
        cursor_state.y = -4;
    if (cursor_state.y > 60)
        cursor_state.y = 60;

    cursor_state.clicking = buttons.R6;
}

void draw_menu_cursor(void)
{
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

    if (buttons.R6)
    {
        fetch_tile_from_tilemap_2bpp(10);
    }
    else
    {
        fetch_tile_from_tilemap_2bpp(base_tile);
    }
    draw_tile_2bpp((unsigned char)cursor_state.x, (unsigned char)cursor_state.y);
}

// =============================================================================
// SCREEN RENDERING FUNCTIONS
// =============================================================================

void debug_io_board(void)
{
    static uint8_t prev_R6 = 0;
    
    unsigned char X_window_1 = 0;
    unsigned char Y_window_1 = 16;
    unsigned char X_window_2 = 80;
    unsigned char Y_window_2 = 16;
    unsigned char X_window_3 = 8;
    unsigned char Y_window_3 = 8;

    // Draw main windows
    draw_window(X_window_2, Y_window_2, 6, 3);
    draw_window(X_window_1, Y_window_1, 8, 3);

    draw_printf(X_window_1 + 3, Y_window_1 + 2, "X:%d\nY:%d\nS:%d", joystick_pos.X >> 8, joystick_pos.Y >> 8, slider);
    draw_printf(X_window_1 + 32, Y_window_1 + 2, "X':%d\nY':%d\nS':%d", touch_pad.x, touch_pad.y, touch_pad.size);

    joystick_indicator(X_window_2 + 8, Y_window_2 + 8, 0);
    joystick_indicator(X_window_2 + 32, Y_window_2 + 8, 1);

    draw_menu_item(27, 3, "Debug IO-BOARD", 15);
    draw_rectangle(23, 1, 86, 12);

    button_indicator(0,42,0,7);
    button_indicator(16,42,0,6);
    button_indicator(32,42,0,5);
    button_indicator(48,42,0,4);
    button_indicator(64,42,0,3);
    button_indicator(80,42,0,2);
    button_indicator(96,42,0,1);

    button_indicator(16,53,1,6);
    button_indicator(32,53,1,5);
    button_indicator(48,53,1,4);
    button_indicator(64,53,1,3);
    button_indicator(80,53,1,2);
    button_indicator(96,53,1,1);

    // Show prompt overlay if not dismissed
    if (!debug_io_prompt_dismissed) {
        Shade_Background();
        draw_window(X_window_3, Y_window_3, 13, 5);
        draw_printf(X_window_3 + 3, Y_window_3 + 3, "To exit: set slider\nto 255 and press R4");

        draw_printf(X_window_3 + 67, Y_window_3 + 24, "OK");
        button_indicator(X_window_3 + 80, Y_window_3 + 24, 1, 6);
        
        // Wait for R6 to be released first
        if (!buttons.R6 && prev_R6) {
            debug_io_allow_dismiss = 1;
        }
        
        // Check for R6 press (OK button) - only after release
        if (buttons.R6 && !prev_R6 && debug_io_allow_dismiss) {
            debug_io_prompt_dismissed = 1;
        }
    }
    
    prev_R6 = buttons.R6;
}

void debug_blue_box(void)
{
    unsigned char Y_window_1 = 16;
    unsigned char Y_window_2 = 42;
    unsigned char Y_window_3 = 16;
    unsigned char X_window_1 = 0;
    unsigned char X_window_2 = 0;
    unsigned char X_window_3 = 63;

    draw_window(X_window_1, Y_window_1, 7, 3);
    draw_window(X_window_2, Y_window_2, 7, 2);
    draw_window(X_window_3, Y_window_3, 8, 6);
    
    draw_printf(X_window_1 + 3, Y_window_1 + 4, "X:%d\nY:%d", joystick_pos.X >> 8, joystick_pos.Y >> 8);
    draw_printf(X_window_2 + 3, Y_window_2 + 1, "ID: 0X%02X\nLEN:0X%02X", msgCAN_RX.message_id, msgCAN_RX.message_data_length);
   
    // First 4 bytes
    uint8_t first_col_count = (msgCAN_RX.message_data_length > 4) ? 4 : msgCAN_RX.message_data_length;
    for (uint8_t i = 0; i < first_col_count; i++) {
        draw_printf(X_window_3 + 3, Y_window_3 + 1 + (i * 7), "0X%02X", msgCAN_RX.message_data[i]);
    }
    
    // Next 4 bytes
    if (msgCAN_RX.message_data_length > 4) {
        for (uint8_t i = 4; i < msgCAN_RX.message_data_length; i++) {
            draw_printf(X_window_3 + 32, Y_window_3 + 1 + ((i - 4) * 7), "0X%02X", msgCAN_RX.message_data[i]);
        }
    }

    draw_line(X_window_3, Y_window_3 + 32, 64, 0, 1);
    draw_printf(X_window_3 + 3, Y_window_3 + 36, "SCORE: %d", score);

    joystick_indicator(X_window_1 + 39, Y_window_1 + 8, 0);

    draw_menu_item(27, 3, "Debug Blue Box", 14);
    draw_rectangle(23, 1, 86, 12);
}

void calibration_joystick(void)
{
    static CalState cal_state = CAL_CENTER;
    static uint8_t prev_buttons = 0;
    static unsigned char direction_show = NEUTRAL;
    
    uint8_t curr_buttons = (buttons.L6 ? 0x01 : 0) | 
                          (buttons.L7 ? 0x02 : 0) | 
                          (buttons.R4 ? 0x04 : 0);
    uint8_t pressed = curr_buttons & ~prev_buttons;
    
    const unsigned char X1 = 8, Y1 = 16;
    const unsigned char X2 = 63, Y2 = 16;
    const unsigned char X3 = 16, Y3 = 8;
    
    // Common header
    draw_menu_item(27, 3, "Joystick Cal. ", 46);
    draw_rectangle(23, 1, 80, 12);
    
    draw_window(X1, Y1, 6, 6);
    draw_window(X2, Y2, 7, 6);
    
    // Left panel - instructions
    draw_printf(X1 + 3, Y1 + 3, "Do:");
    draw_joystick_direction(X1 + 20, Y1 + 13, direction_show, 0);
    draw_printf(X1 + 3, Y1 + 27, "Press:");
    button_indicator(X1 + 16, Y1 + 35, 0, 6);
    
    // Right panel - live values
    draw_printf(X2 + 3, Y2 + 3, "Center:\n(%d,%d)", joy_x_center, joy_y_center);
    draw_printf(X2 + 3, Y2 + 22, "X:%d-%d", joy_x_min, joy_x_max);
    draw_printf(X2 + 3, Y2 + 30, "Y:%d-%d", joy_y_min, joy_y_max);
    
    // State machine
    const JoyDir directions[] = {NEUTRAL, LEFT, RIGHT, DOWN, UP};
    const unsigned char* adc_ptrs[] = {NULL, &adc_ch3, &adc_ch3, &adc_ch2, &adc_ch2};
    unsigned char* cal_ptrs[] = {NULL, &joy_x_min, &joy_x_max, &joy_y_min, &joy_y_max};
    
    if (cal_state < CAL_DONE) {
        direction_show = directions[cal_state];
        
        if (cal_state == CAL_CENTER) {
            joy_x_center = adc_ch3;
            joy_y_center = adc_ch2;
        } else {
            *cal_ptrs[cal_state] = *adc_ptrs[cal_state];
        }
        
        if (pressed & 0x01) {
            cal_state++;
        }
    } else {
        // CAL_DONE state
        direction_show = joystick_dir;
        Shade_Background();
        draw_window(X3, Y3, 13, 5);
        draw_printf(X3 + 3, Y3 + 3, "Center:(%d,%d)", joy_x_center, joy_y_center);
        draw_printf(X3 + 3, Y3 + 11, "X:%d-%d Y:%d-%d", joy_x_min, joy_x_max, joy_y_min, joy_y_max);
        
        draw_printf(X3 + 3, Y3 + 24, "Redo");
        button_indicator(X3 + 25, Y3 + 24, 0, 7);
        draw_printf(X3 + 61, Y3 + 24, "Done");
        button_indicator(X3 + 83, Y3 + 24, 1, 4);
        
        if (pressed & 0x02) {
            joy_x_center = 128; joy_y_center = 128;
            joy_x_min = 0; joy_x_max = 255;
            joy_y_min = 0; joy_y_max = 255;
            Joystick_Init();
            cal_state = CAL_CENTER;
        } else if (pressed & 0x04) {
            current_screen = SCREEN_MENU;
            cal_state = CAL_CENTER;
            Joystick_Init();
        }
    }
    
    prev_buttons = curr_buttons;
}

void startup(void)
{
    draw_sprite_1bpp(logo1bpp_big, 32, 5);
    draw_printf(35, 55, "Start");
    button_indicator(65, 55, 1, 4);
    draw_menu_cursor();
    update_cursor_position();
}

// =============================================================================
// SCREEN MANAGER
// =============================================================================

void display_current_screen(void) 
{
    static unsigned char prev_screen = SCREEN_STARTUP;
    
    // Reset prompt when entering debug screen
    if (current_screen == SCREEN_DEBUG_IO_BOARD && prev_screen != SCREEN_DEBUG_IO_BOARD) {
        debug_io_prompt_dismissed = 0;
        debug_io_allow_dismiss = 0;
    }
    
    switch (current_screen) {
        case SCREEN_MENU:
            if (current_menu == NULL) {
                current_menu = &main_menu;
            }
            draw_menu();
            break;
            
        case SCREEN_DEBUG_IO_BOARD:
            debug_io_board();
            if (buttons.R4 && !prev_R4 && slider == 255) {
                current_screen = SCREEN_MENU;
            }
            prev_R4 = buttons.R4;
            break;
            
        case SCREEN_DEBUG_BLUE_BOX:
            debug_blue_box();
            if (buttons.R4 && !prev_R4) {
                current_screen = SCREEN_MENU;
                // Send no mode
                msgCAN_TX.message_id = 7;
                msgCAN_TX.message_data_length = 1;
                msgCAN_TX.message_data[0] = 0; 
                CAN_Send_Message(msgCAN_TX);
            }
            prev_R4 = buttons.R4;
            break;
        
        case SCREEN_CALIBRATION_JOYSTICK:
            calibration_joystick();
            if (buttons.R4 && !prev_R4) {
                current_screen = SCREEN_MENU;
                cal_state = CAL_CENTER;
                Joystick_Init();
            }
            prev_R4 = buttons.R4;
            break;
            
        case SCREEN_TILEMAP_1BPP:
            draw_tilemap_1bpp();
            if (buttons.R4 && !prev_R4) {
                current_screen = SCREEN_MENU;
            }
            prev_R4 = buttons.R4;
            break;

        case SCREEN_TILEMAP_2BPP:
            draw_tilemap_2bpp();
            if (buttons.R4 && !prev_R4) {
                current_screen = SCREEN_MENU;
            }
            prev_R4 = buttons.R4;
            break;
        
        case SCREEN_STARTUP:
            startup();
            if (buttons.R4 && !prev_R4) {
                current_screen = SCREEN_MENU;
            }
            prev_R4 = buttons.R4;
            break;

        default:
            current_screen = SCREEN_MENU;
            break;
    }
    
    prev_screen = current_screen;
}