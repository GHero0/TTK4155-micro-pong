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
	
	//test_UART_Transmit();
	//test_UART_Receive();
    int i = 0;

    // Configure pins as output
    DDRA |= (1 << PA1);   // PA0 = Data input to latch
    DDRE |= (1 << PE1);   // PE1 = Latch enable

    while (1)
    {
        i++;
        printf("========== Test cycle 1-%d ==========\n", i);

        // === RESET latch output to LOW ===
        PORTA &= ~(1 << PA1);   // PA0 LOW
        PORTE |= (1 << PE1);    // update latch → output LOW
        _delay_ms(50);
        PORTE &= ~(1 << PE1);   // block
        printf("Reset: Output forced LOW\n");

        // === Step 1: Latch enabled, fast toggles ===
        printf("Step 1: Latch enabled (LED follows PA0)\n");
        PORTE |= (1 << PE1);   // latch enabled
        for (int j = 0; j < 10; j++) {
            PORTA ^= (1 << PA1);  // toggle PA0
            _delay_ms(300);
        }
        PORTE &= ~(1 << PE1);  // block
        printf("Latch blocked (output frozen)\n");

        // === Step 2: Latch blocked, fast toggles ===
        printf("Step 2: Latch blocked (LED should stay fixed)\n");
        for (int j = 0; j < 10; j++) {
            PORTA ^= (1 << PA1);  // toggle PA0
            _delay_ms(300);
        }

        // === Step 3: Latch enabled again ===
        printf("Step 3: Latch enabled, output updates to PA0\n");
        PORTE |= (1 << PE1);   // enable → latch current PA0
        _delay_ms(500);

        // === Step 4: Block again ===
        PORTE &= ~(1 << PE1);  // block
        printf("Step 4: Latch blocked, holding last value\n");

        printf("Cycle %d complete\n\n", i);
        _delay_ms(2000);   // short break before next cycle
	
        printf("========== Test cycle 2-%d ==========\n", i);

        // === RESET latch output to LOW ===
        PORTA &= ~(1 << PA1);   // PA0 LOW
        PORTE |= (1 << PE1);    // update latch → output LOW
        _delay_ms(50);
        PORTE &= ~(1 << PE1);   // block
        printf("Reset: Output forced LOW\n");

        // === Step 1: Latch enabled, fast toggles ===
        printf("Step 1: Latch enabled (LED follows PA0)\n");
        PORTE |= (1 << PE1);   // latch enabled
        for (int j = 0; j < 10; j++) {
            PORTA ^= (1 << PA1);  // toggle PA0
            _delay_ms(300);
        }
        PORTA ^= (1 << PA1);  // toggle PA0
        _delay_ms(300);
        PORTE &= ~(1 << PE1);  // block
        printf("Latch blocked (output frozen)\n");

        // === Step 2: Latch blocked, fast toggles ===
        printf("Step 2: Latch blocked (LED should stay fixed)\n");
        for (int j = 0; j < 10; j++) {
            PORTA ^= (1 << PA1);  // toggle PA0
            _delay_ms(300);
        }

        // === Step 3: Latch enabled again ===
        printf("Step 3: Latch enabled, output updates to PA0\n");
        PORTE |= (1 << PE1);   // enable → latch current PA0
        _delay_ms(500);

        // === Step 4: Block again ===
        PORTE &= ~(1 << PE1);  // block
        printf("Step 4: Latch blocked, holding last value\n");

        printf("Cycle %d complete\n\n", i);
        _delay_ms(2000);   // short break before next cycle
    }
}

