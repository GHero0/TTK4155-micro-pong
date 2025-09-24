#ifndef GLOBALDEC_H
#define GLOBALDEC_H
/*
 * globaldec.h
 * Header file included in main.c for global variable declarations.
 *
 * This file declares global variables that are shared across multiple source files.
 * It is included in main.c and contains only the declarations of global variables,
 * ensuring they can be accessed throughout the program.
 */
#include "constants.h"

volatile char *sram = (char *)0x1400;    // Start address for the SRAM
volatile char *adc = (char *)0x1200;     // Start address for the ADC
unsigned int sram_address_space = 0xC00; // SRAM size 3KB
unsigned int adc_address_space = 0x400;  // ADC size 1KB

// ADC interrupts
char Flag_ADC_ready = 0; // ADC data ready
// ADC channels
unsigned char adc_ch0 = 0;
unsigned char adc_ch1 = 0;
unsigned char adc_ch2 = 0;
unsigned char adc_ch3 = 0;

#endif
