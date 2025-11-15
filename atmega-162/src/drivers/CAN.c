/**
 * @file CAN.c
 * @brief CAN Driver Implementation
 */

// =============================================================================
// INCLUDES
// =============================================================================

// Personal headers
#include "drivers/CAN.h"
#include "config.h"
#include "drivers/SPI.h"

// Libraries
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

CANMessage msgCAN_TX = {0};
CANMessage msgCAN_RX = {0};

// =============================================================================
// CAN FUNCTIONS
// =============================================================================

static void CAN_Read(unsigned char address_byte, unsigned char *buffer, unsigned char lentgh)
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
	
static void CAN_Write(unsigned char address_byte, unsigned char *buffer, unsigned char length)
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

static void CAN_WriteByte(unsigned char address_byte, unsigned char data) {
    CAN_Write(address_byte, &data, 1);
}

static void CAN_Request2Send(unsigned char TX_bits)
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

static unsigned char CAN_ReadStatus(void)
{
    SPI_Select_Slave(3);
    SPI_Write_byte(READ_STATUS);
    unsigned char read_status = SPI_Read_byte();
    SPI_Select_Slave(0);
    return read_status;
}

static void CAN_BitModify(unsigned char address_byte, unsigned char mask, unsigned char data)
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
	
	// Enabling RX0 and RX1 interrupts
	CAN_BitModify(CANINTE, 0x03, 0xFF);

	// uint8_t buffer_CNF1 = {0x03};
	// uint8_t buffer_CNF2 = {0xB2};
	// uint8_t buffer_CNF3 = {0x05};

	CAN_WriteByte(CNF1,0x03);
	CAN_WriteByte(CNF2,0xB2);
	CAN_WriteByte(CNF3,0x05);

	// Set the normal mode
	uint8_t buffer[1] = {0b00000000};
	CAN_Write(CANCTRL, buffer, 1);
	
}

void CAN_Send_Message(CANMessage message_to_send)
{
	// ==== WAITING UNTIL PREVIOUS TRANSMISSION IS DONE =====
	// Wait until TXB0 is cleared
	uint8_t txb0 = 0;
	while (1) {
		CAN_Read(TXB0CTRL, &txb0, 1);
		if ((txb0 & 0x08) == 0) 
		{
			break; // TXREQ cleared => buffer free
		}
	}
	
	// Write the 2 bytes ID inside the corresponding register
	CAN_WriteByte(TXB0SIDH, (message_to_send.message_id >> 3));
	CAN_WriteByte(TXB0SIDL, (message_to_send.message_id << 5));
	
	// Write the 1 byte data length inside the corresponding register
	CAN_WriteByte(TXB0DLC, message_to_send.message_data_length);

	// Write at the TX0BCTRL register the message to send
	CAN_Write(TXB0D0, (uint8_t*) message_to_send.message_data, message_to_send.message_data_length);

	CAN_Request2Send(TXB0);
}

CANMessage CAN_Receive_Message()
{
	CANMessage received_message;
	
	uint8_t buffer_read[13] = {0};
	CAN_Read(RXB0SIDH, buffer_read, 13); 
	
	// Receive ID by reconstructing the full ID
	uint16_t shifted_id = buffer_read[0];
	shifted_id = ((shifted_id << 3) | (buffer_read[1] >> 5));


	for (uint8_t i = 5; i < 13; i++)
	{
		uint8_t j = i - 5;
		received_message.message_data[j] = buffer_read[i];
	}
	
	uint8_t CANintf = 0;
	CAN_Read(CANINTF, &CANintf, 1);
	if (CANintf & 0x01) {
		CAN_BitModify(CANINTF, 0x01, 0x00);   // clear RX0IF
	}
	// // I have no idea !!
	// uint8_t buffer_read_ = 0;
	// CAN_Read(CANINTF, &buffer_read_, 1);
	
	received_message.message_id = shifted_id;
	received_message.message_data_length =  buffer_read[4];;

	return received_message;
}


// =============================================================================
// CAN DEBUGGING FUNCTIONS
// =============================================================================

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
	printf("\n");
}

void CAN_Read_TX_Buffer(void)
{
    printf("\n===== TX BUFFER 0 (TXB0) =====\n");
    
    // Read TXB0 Control Register
    uint8_t txb0ctrl;
    CAN_Read(TXB0CTRL, &txb0ctrl, 1);
    printf("TXB0CTRL: 0x%02X\n", txb0ctrl);
    printf("  TXREQ (bit 3): %d %s\n", 
           (txb0ctrl >> 3) & 0x01,
           (txb0ctrl & 0x08) ? "(Transmission pending)" : "(Buffer free)");
    printf("  TXERR (bit 4): %d %s\n", 
           (txb0ctrl >> 4) & 0x01,
           (txb0ctrl & 0x10) ? "(Error detected)" : "");
    printf("  MLOA  (bit 5): %d %s\n", 
           (txb0ctrl >> 5) & 0x01,
           (txb0ctrl & 0x20) ? "(Message lost arbitration)" : "");
    printf("  ABTF  (bit 6): %d %s\n", 
           (txb0ctrl >> 6) & 0x01,
           (txb0ctrl & 0x40) ? "(Message aborted)" : "");
    
    // Read ID registers
    uint8_t sidh, sidl;
    CAN_Read(TXB0SIDH, &sidh, 1);
    CAN_Read(TXB0SIDL, &sidl, 1);
    
    // Reconstruct 11-bit ID
    uint16_t id = (sidh << 3) | (sidl >> 5);
    
    printf("\nTXB0SIDH: 0x%02X\n", sidh);
    printf("TXB0SIDL: 0x%02X\n", sidl);
    printf("Message ID: 0x%03X (%u)\n", id, id);
    
    // Read DLC
    uint8_t dlc;
    CAN_Read(TXB0DLC, &dlc, 1);
    uint8_t data_length = dlc & 0x0F;
    printf("\nTXB0DLC: 0x%02X\n", dlc);
    printf("Data Length: %u bytes\n", data_length);
    printf("RTR (bit 6): %d %s\n", 
           (dlc >> 6) & 0x01,
           (dlc & 0x40) ? "(Remote Transmission Request)" : "");
    
    // Read data bytes
    uint8_t data[8];
    CAN_Read(TXB0D0, data, 8);
    
    printf("\nData bytes:\n");
    for(uint8_t i = 0; i < 8; i++) {
        printf("  TXB0D%u: 0x%02X", i, data[i]);
        if (i < data_length) {
            printf(" (%u)", data[i]);
        } else {
            printf(" (not used)");
        }
        printf("\n");
    }
    printf("================================\n\n");
}