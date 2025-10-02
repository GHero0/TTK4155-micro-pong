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

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdlib.h>

#include <tests/test_IO_board.h>

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

    int X = 45;
    int Y = 25;
    int x = 0;
    int y = 0;
    while (1)
    {
        IO_board_update();

        // printf("\033[2J\033[H%02X %02X %02X %3d %3d %3d %3d\n",
        //    buttons.right, buttons.left, buttons.nav,
        //    slider, touch_pad.x, touch_pad.y, touch_pad.size);
        // printf("L^ R^ N^ SL^ TX^ TY^ TS^");

        FrameBufferClear();
        for (unsigned short i = 0; i < 512; i++)
        {
            current_buffer[i] = 0xFF;
        }
        draw_sprite_1bpp(logo_1bpp, X, Y);
        draw_sprite_2bpp(logo_2bpp, x, y, 1);

        if (Flag_screen == 1)
        {
            FrameBufferPush();
            FrameBufferSwap();
            Flag_screen = 0;
        }
        Joystick_convert();
        switch (joystick_dir)
        {
        case UP:
            Y = (Y - 1);
            break;
        case DOWN:
            Y = (Y + 1);
            break;
        case LEFT:
            X = (X - 1);
            break;
        case RIGHT:
            X = (X + 1);
            break;
        default:
            break;
        }

        if (buttons.NU)
        {
            y--;
        }
        if (buttons.ND)
        {
            y++;
        }
        if (buttons.NL)
        {
            x--;
        }
        if (buttons.NR)
        {
            x++;
        }
    }
}