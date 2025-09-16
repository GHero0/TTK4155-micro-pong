#include "globaldec.h"
#include "drivers/UART_driver.h"
#include "tests/test_UART_driver.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

int main(void)
{
	USART_Init();
	UART_stdio_init();
	
	test_UART_Transmit();
	test_UART_Receive();
}
