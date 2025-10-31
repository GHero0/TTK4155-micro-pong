#include "tests/test_latch.h"
#include "global.h"

#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>

void test_latch_cycle(unsigned char PIN)
{
	unsigned char i = 1; // Cycle counter

	for (unsigned char cycle = 0; cycle < 2; cycle++, i++)
	{
		printf("========== Test cycle %d ==========\n", i);

		// === RESET latch output to LOW ===
		PORTA &= ~(1 << PIN);
		PORTE |= (1 << PE1);
		_delay_ms(50);
		PORTE &= ~(1 << PE1);
		printf("Reset: Output forced LOW\n");

		// === Step 1: Latch enabled, fast toggles ===
		printf("Step 1: Latch enabled, output = input\n");
		PORTE |= (1 << PE1);
		for (int j = 0; j < 10; j++)
		{
			PORTA ^= (1 << PIN);
			_delay_ms(300);
		}
		if (cycle == 1)
		{
			PORTA ^= (1 << PIN);
			_delay_ms(300);
		}
		PORTE &= ~(1 << PE1);
		printf("Latch blocked, output = Q0\n");

		// === Step 2: Latch blocked, fast toggles ===
		printf("Step 2: Latch blocked, output = Q0\n");
		for (int j = 0; j < 10; j++)
		{
			PORTA ^= (1 << PIN);
			_delay_ms(300);
		}

		// === Step 3: Latch enabled again ===
		printf("Step 3: Latch enabled, output = input\n");
		PORTE |= (1 << PE1);
		_delay_ms(500);

		// === Step 4: Block again ===
		PORTE &= ~(1 << PE1);
		printf("Step 4: Latch blocked, output = Q0\n");

		printf("Cycle %d complete\n\n", i);
		_delay_ms(2000);
	}
}