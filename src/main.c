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
    while (1)
    {
        FrameBufferClear();
    
        joystick_indicator(16,32,0);   
        joystick_indicator(104,32,1);   
        // button_indicator(X,Y,0,7);
        
        
        
        if (Flag_screen)
        {
            draw_task_bar();
            draw_window(64,0,3,3);
            draw_string(
                "[Hey]",X,Y);
            // draw_line(0,0,X,Y);
            Flag_screen = 0;
        }
        
                    FrameBufferPush();
                    FrameBufferSwap();
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