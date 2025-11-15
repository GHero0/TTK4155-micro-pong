/**
 * @file SRAM.c
 * @brief External memory enable
 */

// =============================================================================
// INCLUDES
// =============================================================================

// Personal headers
#include "SRAM.h"
#include "config.h"

// Libraries
#include <avr/io.h>

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

volatile char *adc = (char *)ADC_BASE_ADDR; // Start address for the ADC

// =============================================================================
// EXTERNAL SRAM FUNCTIONS
// =============================================================================

void SRAM_Init(void)
{
    // Configure external memory
    MCUCR |= (1 << SRE); // Enable external SRAM interface
}