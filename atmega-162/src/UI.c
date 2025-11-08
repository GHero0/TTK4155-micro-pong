#include "UI.h"
#include "sprites.h"
#include "images.h"
#include "drivers/IO_board.h"
#include "drivers/OLED.h"
#include "joystick.h"
#include "menu.h"
#include "inter.h"
#include "drivers/CAN.h"

#include <stdio.h>
#include <avr/pgmspace.h>
#include <string.h>


ScreenState current_screen = SCREEN_MENU;
CursorState cursor_state = {0, 0, 0};

// Transition system
ScreenTransition screen_transition = {
    .state = TRANSITION_NONE,
    .frame = 0,
    .total_frames = 4,  // Reduced to 4 frames for better performance
    .target_screen = SCREEN_MENU
};

// Scoring
unsigned char score = 0;

// Initialize transition system
void Transition_Init(void) {
    screen_transition.state = TRANSITION_NONE;
    screen_transition.frame = 0;
    screen_transition.total_frames = 4;  // Faster animation
    screen_transition.target_screen = current_screen;
}

// Start a transition to a new screen
void Transition_Start(ScreenState new_screen) {
    if (new_screen != current_screen) {
        screen_transition.state = TRANSITION_OUT;
        screen_transition.frame = 0;
        screen_transition.target_screen = new_screen;
    }
}

// Check if transition is active
uint8_t Transition_Is_Active(void) {
    return (screen_transition.state != TRANSITION_NONE);
}

// Update transition state (call each frame)
void Transition_Update(void) {
    if (screen_transition.state == TRANSITION_NONE) {
        return;
    }
    
    if (screen_transition.state == TRANSITION_OUT) {
        // Fading out current screen
        screen_transition.frame++;
        if (screen_transition.frame >= screen_transition.total_frames) {
            // Switch to new screen and start fade in
            current_screen = screen_transition.target_screen;
            screen_transition.state = TRANSITION_IN;
            screen_transition.frame = 0;
        }
    } else if (screen_transition.state == TRANSITION_IN) {
        // Fading in new screen
        screen_transition.frame++;
        if (screen_transition.frame >= screen_transition.total_frames) {
            // Transition complete
            screen_transition.state = TRANSITION_NONE;
            screen_transition.frame = 0;
        }
    }
}

void Transition_Apply_Dither(uint8_t intensity) {
    if (intensity == 0) return;
    
    if (intensity >= 32) {
        memset(current_buffer, 0, 1024);
        return;
    }
    
    for (uint16_t i = 0; i < 1024; i++) {
        if ((i & 0x1F) >= (32 - intensity)) {
            current_buffer[i] = 0;
        }
    }
}

void draw_window(int X, int Y, unsigned char width_in_tiles, unsigned char height_in_tiles)
{
    // Draw window content area background (black tiles)
    for (unsigned char row = 0; row < height_in_tiles; row++)
    {
        fetch_tile_from_tilemap_1bpp(59); // Black background
        for (unsigned char col = 0; col < width_in_tiles; col++)
        {
            draw_tile_1bpp(X + (col << 3), Y + (row << 3));
        }
    }
    
    // Draw window border rectangle
    draw_rectangle(X, Y, width_in_tiles, height_in_tiles);
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

void debug_io_board(void)
{
    unsigned char Y_window_1 = 0;
    unsigned char Y_window_2 = 40;
    unsigned char X_window_1 = 8;
    unsigned char X_window_2 = 8;

    if (Y > 0) {
        draw_line(127, 0, Y, 1, 1);  // vertical from (127, 0) down Y pixels
    }
    if (X < 127) {
        draw_line(X, Y, 127 - X, 0, 2);  // horizontal from (X, Y) to (127, Y)
    }

    draw_window(X_window_2, Y_window_2, 6, 3);
    draw_window(X_window_1, Y_window_1, 8, 4);

    draw_printf(X_window_1 + 3, Y_window_1 + 8, "X:%d\nY:%d", joystick_pos.X >> 8, joystick_pos.Y >> 8);
    draw_printf(X_window_1 + 30, Y_window_1 + 8, "X':%d\nY':%d\nS:%d", X, Y, touch_pad.size);

    joystick_indicator(X_window_2 + 8, Y_window_2 + 8, 0);
    joystick_indicator(X_window_2 + 32, Y_window_2 + 8, 1);

    draw_printf(60,54,"Debug IO-BOARD");
    // Only update cursor if not transitioning
    if (!Transition_Is_Active()) {
        update_cursor_position();
    }
    draw_menu_cursor();
}

void debug_blue_box(void)
{
    unsigned char Y_window_1 = 0;
    unsigned char Y_window_2 = 24;
    unsigned char Y_window_3 = 0;
    unsigned char Y_window_4 = 48;
    unsigned char X_window_1 = 8;
    unsigned char X_window_2 = 8;
    unsigned char X_window_3 = 72;
    unsigned char X_window_4 = 8;

    

    draw_window(X_window_1, Y_window_1, 7, 3);
    draw_window(X_window_2, Y_window_2, 7, 3);
    draw_window(X_window_3, Y_window_3, 5, 6);
    draw_window(X_window_4, Y_window_4, 6, 2);
    
    draw_printf(X_window_1 + 3, Y_window_1 + 4, "X:%d\nY:%d", joystick_pos.X >> 8, joystick_pos.Y >> 8);
    draw_printf(X_window_2 + 3, Y_window_2 + 4, "ID: 0X%02X\nLEN:0X%02X", msgCAN_RX.message_id, msgCAN_RX.message_data_length);
    for (uint8_t i = 0; i < msgCAN_RX.message_data_length; i++) {
        draw_printf(X_window_3 + 3, Y_window_3 + 4 + (i * 7), "0X%02X", msgCAN_RX.message_data[i]);
    }
    draw_printf(X_window_4 + 3, Y_window_4 + 4, "SCORE:%d", score);


    joystick_indicator(X_window_1 + 36, Y_window_1 + 8, 0);
    draw_printf(60, 56, "Blue Box Debug");
}

void display_current_screen(void) {
    // Initialize menus on first call
    static unsigned char prev_back_button = 0;
    
    // Always draw the current screen content first
    switch (current_screen) {
        case SCREEN_MENU:
            if (current_menu == NULL) {
                current_menu = &main_menu;
            }
            draw_menu();
            break;
            
        case SCREEN_DEBUG_IO_BOARD:
            debug_io_board();
            prev_back_button = 0;
            if (buttons.R5 && !prev_back_button) {
                Transition_Start(SCREEN_MENU);
            }
            prev_back_button = buttons.R5;
            break;
            
        case SCREEN_DEBUG_BLUE_BOX:
            debug_blue_box();
            
            if (Flag_CAN == 1)
            {
                msgCAN_RX = CAN_Receive_Message();
                if (msgCAN_RX.message_id == 1){
                    score = msgCAN_RX.message_data[0];
                }
                Flag_CAN = 0;
            }
            prev_back_button = 0;
            if (buttons.R5 && !prev_back_button) {
                Transition_Start(SCREEN_MENU);
            }
            prev_back_button = buttons.R5;
            break;
            
        default:
            current_screen = SCREEN_MENU;
            break;
    }
    
    // Apply transition effect AFTER drawing (if active)
    if (Transition_Is_Active()) {
        uint8_t dither_intensity;
        
        if (screen_transition.state == TRANSITION_OUT) {
            // Fade out: intensity increases from 0 to 64
            // frame 0: 0, frame 1: 10, ... frame 5: 64
            dither_intensity = ((screen_transition.frame + 1) * 64) / screen_transition.total_frames;
        } else { // TRANSITION_IN
            // Fade in: intensity decreases from 64 to 0
            // frame 0: 64, frame 1: 53, ... frame 5: 0
            dither_intensity = (64 * (screen_transition.total_frames - screen_transition.frame)) / screen_transition.total_frames;
        }
        
        // Only apply dithering if intensity > 0
        if (dither_intensity > 0) {
            Transition_Apply_Dither(dither_intensity);
        }
        
        // Update transition state for next frame
        Transition_Update();
    }
}