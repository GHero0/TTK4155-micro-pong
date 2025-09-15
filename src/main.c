#include "globaldec.h"
#include "drivers/UART_driver.h"
#include "tests/test_UART_driver.h"
#include "tests/test_latch.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include <stdlib.h>
void SRAM_test(void)
{
	volatile char *ext_ram = (char *)0x1400; // Start address for the SRAM
	uint16_t ext_ram_size = 0x800;
	uint16_t write_errors = 0;
	uint16_t retrieval_errors = 0;
	printf("Starting SRAM test...\n");
	// rand() stores some internal state, so calling this function in a loop will
	// yield different seeds each time (unless srand() is called before this function)
	uint16_t seed = rand();
	// Write phase: Immediately check that the correct value was stored
	srand(seed);
	for (uint16_t i = 0; i < ext_ram_size; i++)
	{
		uint8_t some_value = rand();
		ext_ram[i] = some_value;
		uint8_t retreived_value = ext_ram[i];
		if (retreived_value != some_value)
		{
			printf("Write phase error: ext_ram[%4d] = %02X (should be %02X)\n", i, retreived_value, some_value);
			write_errors++;
		}
	}
	// Retrieval phase: Check that no values were changed during or after the write phase
	srand(seed);
	// reset the PRNG to the state it had before the write phase
	for (uint16_t i = 0; i < ext_ram_size; i++)
	{
		uint8_t some_value = rand();
		uint8_t retreived_value = ext_ram[i];
		if (retreived_value != some_value)
		{
			printf("Retrieval phase error: ext_ram[%4d] = %02X (should be %02X)\n", i, retreived_value, some_value);
			retrieval_errors++;
		}
	}
	printf("SRAM test completed with \n%4d errors in write phase and \n%4d errors in retrieval phase\n\n", write_errors, retrieval_errors);
}

int main(void)
{
	USART_Init();
	UART_stdio_init();
	
	//test_UART_Transmit();
	//test_UART_Receive();
	int i = 0;

	// Configure external memory
	MCUCR |= (1 << SRE); // Enable external SRAM interface
	// Pins configuration will be overwritten so we don't have to deal with it
	while (1)
	{
		SRAM_test();
		// volatile char *ext_ram = (char *)0x0000; // Start address
		// ext_ram[0x13FF] = 'z';
		// uint8_t retreived_value = ext_ram[0x13FF];
		//printf("Writing to 0b0001 0000 0000 0000 \n");
		//printf(" Writing 'z'\n");
		//printf("Reading %c\n",retreived_value);
		// _delay_ms(1000);
	}
}
