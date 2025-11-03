#include "SRAM.h"

#include <avr/io.h>

volatile char *adc = (char *)0x1000;     // Start address for the ADC

void SRAM_Init(void)
{
    // Configure external memory
    MCUCR |= (1 << SRE); // Enable external SRAM interface
}