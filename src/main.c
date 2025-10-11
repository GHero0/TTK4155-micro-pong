#include "globaldec.h"
#include "SRAM.h"
#include "inter.h"
#include "PWM.h"
#include "joystick.h"
#include "drivers/UART_driver.h"
#include "drivers/SPI.h"
#include "drivers/IO_board.h"
#include "drivers/OLED.h"
#include "sprites.h"
#include "images.h"
#include "UI.h"

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdlib.h>

void draw_printf(char x, char y, const char* fmt, ...) {
    char buf[32]; // adjust as needed (keep small on AVR)
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    draw_string(buf, x, y);
}


int main(void)
{
    // CLKPR = (1 << CLKPCE);           // Enable prescaler change
    // CLKPR = (1 << CLKPS0);           // Set prescaler to /2 (CLKPS = 0001)
    PWM_init();
    USART_Init();
    UART_stdio_init();
    SRAM_Init();
    Inter_Init();
    Joystick_init();
    SPI_Init();
    OLED_Init();
    FrameBufferInit();

    FrameBufferClear();
    FrameBufferSwap();
    FrameBufferClear();
    FrameBufferPush();

    int X = 32;
    int Y = -64;
    unsigned char step = 0;
    char c = 0;
    signed char y = -16;
    Y_window_1 = -64;
    Y_window_2 = -64;
    X_window_1 = 16;
    X_window_2 = 64;
    while (1)
    {
        FrameBufferClear();

        
        if (Flag_screen)
        {
            draw_line(127,0,X,Y);

            if (step < 10){
                step++;
            } else {
                step = 0;
            }
            if ( Y_window_1 < 8){
                Y_window_1+=2;
            }
            if (Y_window_2 < 16){
                Y_window_2+=2;
            }
            draw_window(X_window_2,Y_window_2,6,3);
            draw_window(X_window_1,Y_window_1,5,4);
            // button_indicator(X,Y,0,7);
            
            // button_indicator(64,8,1,6);
            draw_printf(X_window_1+8, Y_window_1+16, "X:%d\nY:%d", joystick_pos.X >> 8, joystick_pos.Y >> 8);
            draw_task_bar();
            
            
            joystick_indicator(X_window_2+8,Y_window_2+16, 0);
            joystick_indicator(X_window_2+32,Y_window_2+16, 1);
            cursor();
            FrameBufferPush();
            FrameBufferSwap();

            Flag_screen = 0;
        }

        IO_board_update();
        Joystick_convert();

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
}