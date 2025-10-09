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

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdint.h>

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

	// --- LOOPBACK-MODE --- //	
	printf("\nreset :\n");
	CAN_Read_Print_All_Control_Registers();
	_delay_ms(1000);
	
	// --- LOOPBACK-MODE --- //
	uint8_t buffer_loopback_mode[1] = {0b01000111};
	CAN_Write(0x0F, buffer_loopback_mode, 1);
	printf("\n--- Loopback-Mode ---\n");
	CAN_Read_Print_All_Control_Registers();
	
	_delay_ms(1000);
	
	// --- SEND MESSAGE --- //
	uint16_t msg_id = 0x7F0F; // {0x1010 0101 0000 1111};
	char msg_data_length = 8;
	char msg_data[8] = {0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	messageCAN_t msgCAN;
	msgCAN.message_id = msg_id;
	msgCAN.message_data_length = msg_data_length;
	msgCAN.message_data = msg_data;
	CAN_Send_Message(msgCAN);
	
    while (1)
    {
		
    }
}