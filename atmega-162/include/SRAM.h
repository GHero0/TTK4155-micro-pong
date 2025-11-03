#ifndef SRAM_H
#define SRAM_H

// Initialize SRAM
void SRAM_Init(void);

// Start address for the ADC
extern volatile char *adc;

#endif