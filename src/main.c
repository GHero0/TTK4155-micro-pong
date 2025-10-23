#include "globaldec.h"
#include "drivers/UART_driver.h"
#include "SRAM.h"
#include "inter.h"
#include "PWM.h"
#include "joystick.h"
#include "drivers/SPI.h"
#include "tests/test_UART_driver.h"
#include "tests/test_latch.h"
#include "tests/test_SRAM.h"
#include "tests/test_inter.h"
#include "tests/test_adc.h"
#include "tests/test_joystick.h"
#include "drivers/CAN.h"

#include "tests/test_CAN.h"
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

	_delay_ms(50);
	CAN_Reset();
	_delay_ms(50);

	test_CAN_static();
		
	Flag_CAN = 1;
	
	uint16_t msg_id = 0x001;
	char msg_data_length = 8;
	char msg_data[8] = {0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	messageCAN_t msgCAN;
	msgCAN.message_id = msg_id;
	msgCAN.message_data_length = msg_data_length;
	msgCAN.message_data = msg_data;
	
	while(1)
	{
		CAN_Send_Message(msgCAN);
		_delay_ms(500);
			
		if(Flag_CAN == 1)
		{
			printf("INTERRUPT HERE\n");
			CAN_Receive_Message();
			_delay_ms(1);
			Flag_CAN = 0;	
		}
	}
}