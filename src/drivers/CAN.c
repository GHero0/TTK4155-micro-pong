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
	
	SPI_Write_byte(address_byte);	
	for (uint8_t i = 0; i < n; i++)
	{
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
	CAN_Write_byte(0x31, (message_to_send.message_id >> 3));
	CAN_Write_byte(0x32, (message_to_send.message_id << 5));
	
	// Write the 1 byte data length inside the corresponding register
	CAN_Write_byte(0x35, message_to_send.message_data_length);

	// Write at the TX0BCTRL register the message to send
	CAN_Write(0x36, (uint8_t*) message_to_send.message_data, message_to_send.message_data_length);

	// Request to send on TX0
	CAN_Request_To_Send(0x01);
	
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
	for (uint8_t i = 5; i < 12; i++)
	{
		uint8_t j = i - 5;
		received_message.message_data[j] = buffer_read[i];
	}
	
	uint8_t CANintf = 0;
	CAN_Read(CANINTF, &CANintf, 1);
	if (CANintf & 0x01) {
		CAN_Bit_Modify(CANINTF, 0x01, 0x00);   // clear TX0IF
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