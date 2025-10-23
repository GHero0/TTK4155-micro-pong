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

    _delay_ms(50);
	CAN_Reset();
	_delay_ms(50);

    FrameBufferClear();
    FrameBufferSwap();
    FrameBufferClear();
    FrameBufferPush();

    // test_CAN_static();

    // --- LOOPBACK-MODE --- //
	uint8_t buffer_loopback_mode[1] = {0b01000000};
	CAN_Write(0x0F, buffer_loopback_mode, 1);
	printf("\n--- Loopback-Mode ---\n");
	CAN_Read_Print_All_Control_Registers();
	
	_delay_ms(1000);
	
	/* --- ERASE THIS (Since it's in CAN_Init() function --- */
	// Enabling RX0 and RX1 interrupts
	CAN_BitModify(CANINTE, 0x03, 0xFF);

    Flag_CAN = 1;

    uint16_t msg_id = 0x001;
	char msg_data_length = 8;
	char msg_data[8] = {0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	messageCAN_t msgCAN;
	msgCAN.message_id = msg_id;
	msgCAN.message_data_length = msg_data_length;
	msgCAN.message_data = msg_data;
    

    int X = 32;
    int Y = -64;
    
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
        CAN_Send_Message(msgCAN);
        _delay_ms(1);

        if(Flag_CAN == 1)
		{
			printf("INTERRUPT HERE\n");// Remove for animation purposes
			CAN_Receive_Message();
			_delay_ms(1);
			Flag_CAN = 0;	
		}

        IO_board_update();
        Joystick_convert();

        map_touchpad();
    }
}