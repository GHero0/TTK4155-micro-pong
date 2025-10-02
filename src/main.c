#include "globaldec.h"
#include "SRAM.h"
#include "inter.h"
#include "PWM.h"
#include "joystick.h"
#include "drivers/UART_driver.h"
#include "drivers/SPI.h"
#include "drivers/OLED.h"
#include "sprites.h"
#include "images.h"

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdlib.h>

int main(void)
{
    PWM_init();
    USART_Init();
    UART_stdio_init();
    SRAM_Init();
    Inter_Init();
    Joystick_init();
    OLED_Init();
    FrameBufferInit();

    
    int X = 45; 
    int Y = 25;
    while (1)
    {
        FrameBufferClear();
        for (unsigned short i = 0; i < 512; i++){
            current_buffer[i] = 0xFF;
        }
        draw_sprite_1bpp(logo_1bpp,X,Y);
        draw_sprite_2bpp(logo_2bpp,Y,X,1);

        if (Flag_screen == 1){
            FrameBufferPush();
            FrameBufferSwap();
            Flag_screen = 0;
        }
        Joystick_convert();
        if (joystick_dir == UP)
        {
            Y = (Y - 1);
        }
        else if (joystick_dir == DOWN)
        {
            Y = (Y + 1);
        }
        else if (joystick_dir == LEFT)
        {
            X = (X - 1);
        }
        else if (joystick_dir == RIGHT)
        {
            X = (X + 1);
        }
    }
}