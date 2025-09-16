#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include "global.h"

// UART only 
void USART_Init(void);
void USART_Transmit(unsigned char data);
unsigned char USART_Receive(void);
void USART_Transmit_String (char*String);

// UART & stdio
int UART_stdio_init(void);

	


#endif 