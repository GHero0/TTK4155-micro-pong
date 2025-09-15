/*
 * group 32 project.c
 *
 * Created: 04.09.2025 09:17:49
 * Author : felixla
 */ 
#include "globaldec.h"
#include "UART_driver.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

int main(void)
{
	DDRA = 1;
	int i = 0;
	USART_Init();
	UART_stdio_init();
	char input = '0';
    /* Replace with your application code */
    while (1) 
    {
		// scanf("%c",&input);
		// if (input == '1'){
		// 	PORTA = 0xFF;
		// }
		// if (input == '2'){
		// 	PORTA = 0x00;
		// }
		printf("Test %d\n",i);
		i++;
    }
}
