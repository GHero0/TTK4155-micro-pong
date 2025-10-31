#include "SRAM.h"

#include <avr/io.h>

void SRAM_Init()
{
    // Configure external memory
    MCUCR |= (1 << SRE); // Enable external SRAM interface
}