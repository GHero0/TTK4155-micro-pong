#include "drivers/CAN.h"
#include "drivers/SPI.h"

#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>

void CAN_Read(uint8_t address_byte, uint8_t* buffer, uint8_t n)
{
	// If either the buffer is empty or the length is null, we don't try to execute the reading command
	if((!buffer) || (n == 0))
	{
		return;
	}
	
	// Select the slave : CAN controller
	SPI_Select_Slave(3);

	/* Reading the CAN buses */
	SPI_Write_byte(0x03);
	_delay_us(3);
	
	SPI_Write_byte(address_byte);
	for (uint8_t i = 0; i < n; i++)
	{
		buffer[i] = SPI_Read_byte();
	}
	
	// Release the slave selected
	SPI_Select_Slave(0);
}
	
void CAN_Write(uint8_t address_byte, uint8_t* buffer, uint8_t n)
{
	// If either the buffer is empty or the length is null, we don't try to execute the writing command
	if((!buffer) || (n == 0))
	{
		return;
	}
	
	// Select the slave : CAN controller
	SPI_Select_Slave(3);

	/* Writing the CAN buses */
	SPI_Write_byte(0x02);
	_delay_us(3);
	
	for (uint8_t i = 0; i < n; i++)
	{
		SPI_Write_byte(address_byte + i);
		_delay_us(3);
		SPI_Write_byte(buffer[i]);
	}
	
	// Release the slave selected
	SPI_Select_Slave(0);
}

void CAN_Write_byte(uint8_t address_byte, uint8_t byte)
{	
	// Select the slave : CAN controller
	SPI_Select_Slave(3);

	/* Writing the CAN buses */
	SPI_Write_byte(0x02);
	_delay_us(3);
	
	SPI_Write_byte(address_byte);
	_delay_us(3);
	SPI_Write_byte(byte);
	
	// Release the slave selected
	SPI_Select_Slave(0);
}

void CAN_Request_To_Send(uint8_t TX_mask)
{
	// & operation on both mask and 111 (which enables TXB0-2)
	TX_mask &= 0x07;
	if(TX_mask == 0x00)
	{
		return; // the command is ignored if equals to 0
	}
	
	// Select the slave : CAN controller
	SPI_Select_Slave(3);

	/* Send the request (with the following operation code : 0x80 which is the request to send with TXB0-2 all to 0) */
	SPI_Write_byte(0x80 | TX_mask);
	
	// Release the slave selected
	SPI_Select_Slave(0);
}

uint8_t CAN_Read_Status()
{
	// Select the slave : CAN controller
	SPI_Select_Slave(3);

	/* Read the CAN status and store it into a variable */
	SPI_Write_byte(0xA0);
	uint8_t status = SPI_Read_byte();
	
	// Release the slave selected
	SPI_Select_Slave(0);
	
	return status;
}

void CAN_Bit_Modify(uint8_t address_byte, uint8_t mask_byte, uint8_t data_byte)
{
	// Select the slave : CAN controller
	SPI_Select_Slave(3);

	/* Read the CAN status and store it into a variable*/
	SPI_Write_byte(0x05);
	SPI_Write_byte(address_byte);
	SPI_Write_byte(mask_byte);
	SPI_Write_byte(data_byte);
	
	// Release the slave selected
	SPI_Select_Slave(0);
}

void CAN_Reset()
{
	// Select the slave : CAN controller
	SPI_Select_Slave(3);

	/* Reset the CAN */
	SPI_Write_byte(0xC0);
	
	// Release the slave selected
	SPI_Select_Slave(0);
	_delay_us(10);
}

void CAN_Init()
{
	// Set the normal mode
	uint8_t buffer[1] = {0b00000111};
	CAN_Write(0x0F, buffer, 1);
}

void CAN_Send_Message(messageCAN_t message_to_send)
{
	// Write the 2 bytes ID inside the corresponding register
	CAN_Write_byte(0x31, (message_to_send.message_id >> 3));
	CAN_Write_byte(0x32, (message_to_send.message_id << 5));
	
	// Write the 1 byte data length inside the corresponding register
	CAN_Write_byte(0x35, (message_to_send.message_data_length));

	// Write at the TX0BCTRL register the message to send
	CAN_Write(0x36, message_to_send.message_data, message_to_send.message_data_length);
	
	// Put the TXREQ down to high value : '1'
	CAN_Bit_Modify(TXB0CTRL, 0x08, 0xFF);
	
	// Request to send on TX0
	CAN_Request_To_Send(0x01);
	
	// --- Reading test --- //
	uint8_t buffer_read[14] = {0};
	CAN_Read(RXB0CTRL, buffer_read, 0x0E); // 0X0E = 14 bits
	printf("\n---");
	for(int i = 0; i < 14; i++)
	{
		printf("Test buffer_read[%d] = 0x%02X\n", i, buffer_read[i]);
	}
	// --- READING TEST --- //
}

// void CAN_Send_Message(messageCAN_t message_to_send)
// {
// 	// Write the 2 bytes ID inside the corresponding register
// 	CAN_Write(0x31, message_to_send.message_id, 2);
// 	
// 	// Write the 1 byte data length inside the corresponding register
// 	CAN_Write(0x35, message_to_send.message_data_length, 1);
// 	
// 	// Write at the TX0BCTRL register the message to send
// 	CAN_Write(0x36, message_to_send.message_data, message_to_send.message_data_length[0]);
// 
// 	// Put the TXREQ down to high value : '1'
// 	CAN_Bit_Modify(TXB0CTRL, 0x08, 0xFF);
// 	
// 	// Request to send on TX0
// 	CAN_Request_To_Send(0x01);
// 	
// 	// --- Reading test --- //
// 	uint8_t buffer_read[14] = {0};
// 	CAN_Read(RXB0CTRL, buffer_read, 0x0E); // 0X0E = 14 bits
// 	printf("\n---");
// 	for(int i = 0; i < 14; i++)
// 	{
// 		printf("Test buffer_read[%d] = 0x%02X\n", i, buffer_read[i]);
// 	}
// 	// --- READING TEST --- //
// }

messageCAN_t CAN_Receive_Message()
{
	messageCAN_t received_message;
	
	return received_message;
}

void CAN_Read_Print_All_Control_Registers()
{
	uint8_t buffer[1] = {0};
	CAN_Read(BFPCTRL, buffer, 1);
	printf("0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(TXRTSCTRL, buffer, 1);
	printf("0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(CANSTAT, buffer, 1);
	printf("0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(CANCTRL, buffer, 1);
	printf("0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(TEC, buffer, 1);
	printf("0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(REC, buffer, 1);
	printf("0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(CNF3, buffer, 1);
	printf("0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(CNF2, buffer, 1);
	printf("0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(CNF1, buffer, 1);
	printf("0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(CANINTE, buffer, 1);
	printf("0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(CANINTF, buffer, 1);
	printf("0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(EFLG, buffer, 1);
	printf("0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(TXB0CTRL, buffer, 1);
	printf("0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(TXB1CTRL, buffer, 1);
	printf("0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(TXB1CTRL, buffer, 1);
	printf("0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(RXB0CTRL, buffer, 1);
	printf("0x%02X\n", buffer[0]);
	
	buffer[0] = 0;
	CAN_Read(RXB1CTRL, buffer, 1);
	printf("0x%02X\n", buffer[0]);
}