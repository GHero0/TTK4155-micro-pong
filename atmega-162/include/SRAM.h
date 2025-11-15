/**
 * @file SRAM.h
 * @brief External SRAM Init and adc address space
 */

#ifndef SRAM_H
#define SRAM_H

// =============================================================================
// FUNCTION DECLARATIONS
// =============================================================================

// Initialize SRAM
void SRAM_Init(void);

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

// Start address for the ADC
extern volatile char *adc;

#endif