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

int main(void)
{
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