/**
 * @file UART_driver.h
 * @brief UART Function prototypes and baud rate defines
 */

#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include "config.h"

// UART only 
void UART_Init(void);
void UART_Transmit(unsigned char data);
unsigned char UART_Receive(void);
void UART_Transmit_String (char*String);
unsigned char UART_DataAvailable(void);

// UART & stdio
int UART_Stdio_Init(void);

#define BAUD 9600  
#define MYUBRR ((F_CPU/16/BAUD) - 1) 
	
#endif 