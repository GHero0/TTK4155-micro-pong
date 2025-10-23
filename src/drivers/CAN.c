#include "drivers/CAN.h"
#include "drivers/SPI.h"

#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>

void CAN_Read(unsigned char address_byte, unsigned char *buffer, unsigned char lentgh)
{
    SPI_Select_Slave(3);
    SPI_Write_byte(READ); // Read instruction
    // We then indicate the address at which we want to start reading
    // Reading pointer is automatically incremented sequentially
    SPI_Write_byte(address_byte);
    for (unsigned char i = 0; i < lentgh; i++)
    {
        buffer[i] = SPI_Read_byte();
    }
    SPI_Select_Slave(0);
}
	
void CAN_Write(unsigned char address_byte, unsigned char *buffer, unsigned char length)
{
    SPI_Select_Slave(3);
    SPI_Write_byte(WRITE); // Write instruction
    // We then indicate the address at which we want to start writing
    // Writing pointer is automatically incremented sequentially
    SPI_Write_byte(address_byte);
    for (unsigned char i = 0; i < length; i++)
    {
        SPI_Write_byte(buffer[i]);
    }
    SPI_Select_Slave(0);
}

void CAN_WriteByte(unsigned char address_byte, unsigned char data) {
    CAN_Write(address_byte, &data, 1);
}

void CAN_Request2Send(unsigned char TX_bits)
{
    SPI_Select_Slave(3);
    // RTS for TXB0 === 0001
    // RTS for TXB1 === 0010
    // RTS for TXB2 === 0100
    // To consider only thoose bits we mask out everything else:
    TX_bits &= 0b00000111; // VERY OPTIONNAL
    // Then we apply them to RTS Instruction
    SPI_Write_byte(RTS | TX_bits);
    SPI_Select_Slave(0);
}

unsigned char CAN_ReadStatus(void)
{
    SPI_Select_Slave(3);
    SPI_Write_byte(READ_STATUS);
    unsigned char read_status = SPI_Read_byte();
    SPI_Select_Slave(0);
    return read_status;
}

void CAN_BitModify(unsigned char address_byte, unsigned char mask, unsigned char data)
{
    SPI_Select_Slave(3);
    SPI_Write_byte(BIT_MODIFY);
    SPI_Write_byte(address_byte);
    SPI_Write_byte(mask);
    SPI_Write_byte(data);
    SPI_Select_Slave(0);
}

void CAN_Reset(void)
{
    SPI_Select_Slave(3);
    SPI_Write_byte(RESET); // Reset instruction
    // After sending reset instruction we are put in config mode automatically
    SPI_Select_Slave(0);
    _delay_us(10);
}

void CAN_Init()
{
	// Set the normal mode
	uint8_t buffer[1] = {0b00000111};
	CAN_Write(0x0F, buffer, 1);
	
	// Enabling RX0 and RX1 interrupts
	CAN_BitModify(CANINTE, 0x03, 0xFF);
}

void CAN_Send_Message(messageCAN_t message_to_send)
{
	// Check that data_length is not greater than 8
	if(message_to_send.message_data_length > 8)
	{
		message_to_send.message_data_length = 8;
	}
	// Check if data_length is not equal to 0 (if 0 then we don't try to send antyhing)
	if(message_to_send.message_data_length == 0)
	{
		return;
	}
	
	// Wait until TXB0 is cleared
	uint16_t timeout = 60000;
	while (timeout > 0) {
		uint8_t txb0 = 0;
		CAN_Read(TXB0CTRL, &txb0, 1);
		if ((txb0 & 0x08) == 0) 
		{
			break; // TXREQ cleared => buffer free
		}
		
		timeout--;
	}
	
	// Write the 2 bytes ID inside the corresponding register
	CAN_WriteByte(0x31, (message_to_send.message_id >> 3));
	CAN_WriteByte(0x32, (message_to_send.message_id << 5));
	
	// Write the 1 byte data length inside the corresponding register
	CAN_WriteByte(0x35, message_to_send.message_data_length);

	// Write at the TX0BCTRL register the message to send
	CAN_Write(0x36, (uint8_t*) message_to_send.message_data, message_to_send.message_data_length);

	// Request to send on TX0
	CAN_Request2Send(0x01);
	
	// Check if sending is finished
	timeout = 60000;
	while(timeout > 0)
	{
		uint8_t txb0 = 0;
		CAN_Read(TXB0CTRL, &txb0, 1);
		if ((txb0 & 0x08) == 0)
		{
			 break; // TXREQ==0
		}
		
		uint8_t intf = 0;
		CAN_Read(CANINTF, &intf, 1);
		if (intf & 0x04) {
			break; // TX0IF set
		}
		
		timeout--;
	}
}

messageCAN_t CAN_Receive_Message()
{
	messageCAN_t received_message;
	
	uint8_t buffer_read[13] = {0};
	CAN_Read(RXB0SIDH, buffer_read, 0X0D); // 0X0D = 13
	
	// Loading received_message with ID/Data_length/Datas
	uint16_t shifted_id = buffer_read[0];
	shifted_id = ((shifted_id << 3) | (buffer_read[1] >> 5));
	received_message.message_id = shifted_id;
	received_message.message_data_length = buffer_read[4];
	for (uint8_t i = 5; i < 13; i++)
	{
		uint8_t j = i - 5;
		received_message.message_data[j] = buffer_read[i];
	}
	
	uint8_t CANintf = 0;
	CAN_Read(CANINTF, &CANintf, 1);
	if (CANintf & 0x01) {
		CAN_BitModify(CANINTF, 0x01, 0x00);   // clear TX0IF
	}
	uint8_t buffer_read_ = 0;
	CAN_Read(CANINTF, &buffer_read_, 1);
	
	return received_message;
}

void CAN_Read_Print_All_Control_Registers()
{
	uint8_t buffer[1] = {0};
	CAN_Read(BFPCTRL, buffer, 1);
	printf("BFPCTRL :\t0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(TXRTSCTRL, buffer, 1);
	printf("TXRTSCTRL :\t0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(CANSTAT, buffer, 1);
	printf("CANSTAT :\t0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(CANCTRL, buffer, 1);
	printf("CANCTRL :\t0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(TEC, buffer, 1);
	printf("TEC :\t\t0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(REC, buffer, 1);
	printf("REC :\t\t0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(CNF3, buffer, 1);
	printf("CNF3 :\t\t0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(CNF2, buffer, 1);
	printf("CNF2 :\t\t0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(CNF1, buffer, 1);
	printf("CNF1 :\t\t0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(CANINTE, buffer, 1);
	printf("CANINTE :\t0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(CANINTF, buffer, 1);
	printf("CANINTF :\t0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(EFLG, buffer, 1);
	printf("EFLG :\t\t0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(TXB0CTRL, buffer, 1);
	printf("TXB0CTRL :\t0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(TXB1CTRL, buffer, 1);
	printf("TXB1CTRL :\t0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(TXB2CTRL, buffer, 1);
	printf("TXB2CTRL :\t0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(RXB0CTRL, buffer, 1);
	printf("RXB0CTRL :\t0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(RXB1CTRL, buffer, 1);
	printf("RXB1CTRL :\t0x%02X\n", buffer[0]);
}