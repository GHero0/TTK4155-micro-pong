#include "hardware.h"
#include "tests/test_CAN.h"
#include "drivers/CAN.h"
#include "drivers/SPI.h"
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>

void test_CAN_static(void)
{
    // Reset and print initial state
    CAN_Reset();
    printf("===== RESET ====\n");
    // CAN_Read_Print_All_Control_Registers();
    
    // Set loopback mode
    uint8_t buffer_loopback_mode[1] = {0b01000000};
    CAN_Write(CANCTRL, buffer_loopback_mode, 1);
    
    // Enable RX0 and RX1 interrupts
    CAN_BitModify(CANINTE, 0x03, 0xFF);
    
    printf("==== Loopback-Mode ====\n");
    // CAN_Read_Print_All_Control_Registers();
    
    _delay_ms(100);
    
    printf("Hello there\n");
    
    messageCAN_t msgCAN = {
        .message_id = 0x01,
        .message_data_length = 8,
        .message_data = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08}
    };

    // Send message
	printf("Message about to be sent\n");
    CAN_Send_Message(msgCAN);
	printf("Message sent !\n");
    _delay_ms(100);
    CAN_Read_TX_Buffer();

    
    // Receive message
    messageCAN_t received_msg = CAN_Receive_Message();
    
    // Print received message with CORRECT format specifiers
    printf("====MSG1====\n");
    printf("id1: 0x%03X\n", received_msg.message_id);  // %03X for 11-bit ID (3 hex digits)
    printf("data_length1: 0x%02X\n", received_msg.message_data_length);
    
    for(uint8_t i = 0; i < received_msg.message_data_length; i++)
    {
        printf("data[%d]: 0x%02X\n", i, received_msg.message_data[i]);
    }
}