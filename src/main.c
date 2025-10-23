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

#include "tests/test_UI.h"

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

    
    while (1)
    {
        if (Flag_screen)
        {
            FrameBufferClear();

            debug_window();
            
            FrameBufferPush();
            FrameBufferSwap();

            Flag_screen = 0;
        }

        IO_board_update();
        Joystick_convert();

        map_touchpad();
    }
}