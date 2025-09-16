#include "drivers/UART_driver.h"
#include <avr/io.h>
#include <stdio.h>

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1



void USART_Init(){
    /* Set baud rate */
    UBRR0H = (unsigned char)(MYUBRR >> 8);
    UBRR0L = (unsigned char)(MYUBRR);

    UCSR0B |= (1<<RXEN0)|(1<<TXEN0); // 00011000
    UCSR0C |= (1<<URSEL0)|(3<<UCSZ00);
}

void USART_Transmit(unsigned char data){
    while ( !( UCSR0A & (1<<UDRE0)) );
    UDR0 = data;
}

unsigned char USART_Receive(void){
    while ( !(UCSR0A & (1<<RXC0)) );
    return UDR0;
}

void USART_Transmit_String (char*String){
	while(*String)
	{
		USART_Transmit(*String);
		String++;
	}
}

int uart_putchar(char c, FILE *stream)
{
  if (c == '\n'){
    uart_putchar('\r', stream);
  }
  USART_Transmit(c);
  return 0;
}

int uart_getchar(FILE *stream){
    return USART_Receive();
}

FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL,_FDEV_SETUP_WRITE);

FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

int UART_stdio_init(){
    stdout = &uart_output;
    stdin = &uart_input;
    return 0;
}
