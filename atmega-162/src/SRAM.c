/**
 * @file SRAM.c
 * @brief External memory enable
 */

#include "SRAM.h"
#include "config.h"

#include <avr/io.h>

volatile char *adc = (char *)ADC_BASE_ADDR;     // Start address for the ADC

void SRAM_Init(void)
{
    // Configure external memory
    MCUCR |= (1 << SRE); // Enable external SRAM interface
}