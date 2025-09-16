#include "globaldec.h"
#include "drivers/UART_driver.h"
#include "tests/test_UART_driver.h"
#include "tests/test_latch.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>





int main(void)
{
	USART_Init();
	UART_stdio_init();
	
	//test_UART_Transmit();
	//test_UART_Receive();
    int i = 0;

    // Configure pins as output
    DDRA = 0xFF;   			// PORTA set to output
    DDRE |= (1 << PE1);   	// PE1 = Latch enable

    while (1)
    {
		test_latch_cycle(PA0);
	}
	
	return 0;
}