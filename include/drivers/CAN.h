#ifndef CAN_H_
#define CAN_H_

#include "global.h"

#include <stdint.h>

#define BFPCTRL		0x0C
#define TXRTSCTRL	0x0D
#define CANSTAT		0x0E
#define CANCTRL		0x0F
#define TEC			0x1C
#define REC			0x1D
#define CNF3		0x28
#define CNF2		0x29
#define CNF1		0x2A
#define CANINTE		0x2B
#define CANINTF		0x2C
#define EFLG		0x2D
#define TXB0CTRL	0x30
#define TXB1CTRL	0x40
#define TXB2CTRL	0x50
#define RXB0CTRL	0x60
#define RXB1CTRL	0x70
#define RXB0SIDH	0x61

// Instructions 
#define RESET           0xC0
#define READ            0x03
#define READ_RX_BUFFER  0x90
#define WRITE           0x02
#define LOAD_TX_BUFFER  0x40
#define RTS             0x80
#define READ_STATUS     0xA0
#define RX_STATUS       0xB0
#define BIT_MODIFY      0x05

// CAN Message struct
typedef struct {
	uint16_t message_id;
	char message_data_length;
	char * message_data;
} messageCAN_t;

void CAN_Read(unsigned char address_byte, unsigned char *buffer, unsigned char lentgh);
void CAN_Write(unsigned char address_byte, unsigned char *buffer, unsigned char length);
void CAN_WriteByte(unsigned char address_byte, unsigned char data);
void CAN_Request2Send(unsigned char TX_bits);
unsigned char CAN_ReadStatus(void);
void CAN_BitModify(unsigned char address_byte, unsigned char mask, unsigned char data);
void CAN_Reset(void);
void CAN_Init(void);
void CAN_Send_Message(messageCAN_t);
messageCAN_t CAN_Receive_Message(/*void*/);

void CAN_Read_Print_All_Control_Registers(void);

#endif /* CAN_H_ */