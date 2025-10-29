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
#include "drivers/CAN.h"

#include "tests/test_CAN.h"
#include "tests/test_UI.h"
#include <util/delay.h>
#include <stdio.h>

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
    CAN_Reset();
    _delay_ms(100);

    FrameBufferClear();
    FrameBufferSwap();
    FrameBufferClear();
    FrameBufferPush();

    uint16_t msg_id = 2;
    char msg_data_length = 4;
    char msg_data[8] = {0x11, 0x22, 0x33, 0x44};

    messageCAN_t msgCAN;
    msgCAN.message_id = msg_id;
    msgCAN.message_data_length = msg_data_length;
    msgCAN.message_data = msg_data;

    // ===== CAN INIT =====
    CAN_Init();
    printf("\n");
    CAN_Read_Print_All_Control_Registers();

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

        if (Flag_CAN == 1)
        {
            printf("\n\nINTERRUPT HERE\n\n");
            msgCAN = CAN_Receive_Message();
            _delay_ms(1);
            printf("====MSG====\n");
            printf("id: 0x%X\n", msgCAN.message_id);
            printf("data_length: 0x%X\n", msgCAN.message_data_length);

            for (uint8_t i = 0; i < msgCAN.message_data_length; i++)
            {
                printf("data[%d]: 0x%2X\n", i, msgCAN.message_data[i]);
            }
            Flag_CAN = 0;
        }

        CAN_Send_Message(msgCAN);

        IO_board_update();
        Joystick_convert();

        map_touchpad();
    }
}