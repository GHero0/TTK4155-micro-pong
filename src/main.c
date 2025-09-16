#include "globaldec.h"
#include "drivers/UART_driver.h"
#include "SRAM.h"
#include "tests/test_UART_driver.h"
#include "tests/test_latch.h"
#include "tests/test_SRAM.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

int main(void)
{
	USART_Init();
	UART_stdio_init();
	SRAM_Init();

	while (1)
	{
		test_SRAM();
		test_address_decoder();
	}
}
