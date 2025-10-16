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
	uint8_t buffer_loopback_mode[1] = {0b01000000};
	CAN_Write(0x0F, buffer_loopback_mode, 1);
	printf("\n--- Loopback-Mode ---\n");
	CAN_Read_Print_All_Control_Registers();
	
	_delay_ms(1000);
	
	// --- SEND MESSAGE --- //
	messageCAN_t msgReceived;
	
	uint16_t msg_id = 0x001;
	char msg_data_length = 8;
	char msg_data[8] = {0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	messageCAN_t msgCAN;
	msgCAN.message_id = msg_id;
	msgCAN.message_data_length = msg_data_length;
	msgCAN.message_data = msg_data;
	CAN_Send_Message(msgCAN);
	msgReceived = CAN_Receive_Message();
	printf("--- MSG 1 ---\n");
	printf("id1 : \t\t0x%02X\ndata_length1 : \t0x%02X\n", msgReceived.message_id, msgReceived.message_data_length);
	for (uint8_t i = 0; i < 8; i++)
	{
		printf("data[%d] : \t0x%02X\n", i, msgReceived.message_data[i]);
	}
	printf("\n");
	_delay_ms(1000);

	uint16_t msg_id2 = 0x002;
	char msg_data_length2 = 8;
	char msg_data2[8] = {0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	messageCAN_t msgCAN2;
	msgCAN2.message_id = msg_id2;
	msgCAN2.message_data_length = msg_data_length2;
	msgCAN2.message_data = msg_data2;
	CAN_Send_Message(msgCAN2);
	msgReceived = CAN_Receive_Message();
	printf("--- MSG 2 ---\n");
	printf("id2 : \t\t0x%02X\ndata_length2 : \t0x%02X\n", msgReceived.message_id, msgReceived.message_data_length);
	for (uint8_t i = 0; i < 8; i++)
	{
		printf("data[%d] : \t0x%02X\n", i, msgReceived.message_data[i]);
	}
	printf("\n");
	_delay_ms(1000);
	
	uint16_t msg_id3 = 0x003;
	char msg_data_length3 = 8;
	char msg_data3[8] = {0x00,0x00,0x55,0x00,0x00,0x00,0x00,0x00};
	messageCAN_t msgCAN3;
	msgCAN3.message_id = msg_id3;
	msgCAN3.message_data_length = msg_data_length3;
	msgCAN3.message_data = msg_data3;
	CAN_Send_Message(msgCAN3);
	msgReceived = CAN_Receive_Message();
	printf("--- MSG 3 ---\n");
	printf("id3 : \t\t0x%02X\ndata_length3 : \t0x%02X\n", msgReceived.message_id, msgReceived.message_data_length);
	for (uint8_t i = 0; i < 8; i++)
	{
		printf("data[%d] : \t0x%02X\n", i, msgReceived.message_data[i]);
	}
	printf("\n");
		
    while (1)
    {
		
    }
}