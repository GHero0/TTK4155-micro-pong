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

// CAN Message struct
typedef struct {
	uint16_t message_id;
	char message_data_length;
	char * message_data;
} messageCAN_t;

// CAN Message struct
// typedef struct {
// 	uint8_t * message_id;
// 	uint8_t * message_data_length;
// 	uint8_t * message_data[8];
// } messageCAN_t;

void CAN_Read(uint8_t, uint8_t*, uint8_t);
void CAN_Write(uint8_t, uint8_t*, uint8_t);
void CAN_Write_byte(uint8_t, uint8_t);
void CAN_Request_To_Send(uint8_t);
uint8_t CAN_Read_Status(void);
void CAN_Bit_Modify(uint8_t, uint8_t, uint8_t);
void CAN_Reset(void);
void CAN_Init(void);
void CAN_Send_Message(messageCAN_t);
messageCAN_t CAN_Receive_Message(/*void*/);

void CAN_Read_Print_All_Control_Registers(void);

#endif /* CAN_H_ */