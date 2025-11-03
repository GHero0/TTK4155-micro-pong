/**
 * @file UART_driver.c
 * @brief UART Driver Implementaiton functions along with stdio coupling
 */

#include "drivers/UART_driver.h"

#include <avr/io.h>
#include <stdio.h>

void UART_Init(){
    /* Set baud rate */
    UBRR0H = (unsigned char)(MYUBRR >> 8);
    UBRR0L = (unsigned char)(MYUBRR);

    UCSR0B |= (1<<RXEN0)|(1<<TXEN0); // 00011000
    UCSR0C = (1 << URSEL0) | (1 << UCSZ01) | (1 << UCSZ00);
}

void UART_Transmit(unsigned char data){
    while ( !( UCSR0A & (1<<UDRE0)) );
    UDR0 = data;
}

unsigned char UART_Receive(void){
    while ( !(UCSR0A & (1<<RXC0)) );
    return UDR0;
}

void UART_Transmit_String (char*String){
	while(*String)
	{
		UART_Transmit(*String);
		String++;
	}
}

static int UART_Putchar(char c, FILE *stream)
{
  if (c == '\n'){
    UART_Putchar('\r', stream);
  }
  UART_Transmit(c);
  return 0;
}

static int UART_Getchar(FILE *stream){
    return UART_Receive();
}

FILE uart_output = FDEV_SETUP_STREAM(UART_Putchar, NULL,_FDEV_SETUP_WRITE);

FILE uart_input = FDEV_SETUP_STREAM(NULL, UART_Getchar, _FDEV_SETUP_READ);

int UART_Stdio_Init(){
    stdout = &uart_output;
    stdin = &uart_input;
    return 0;
}
