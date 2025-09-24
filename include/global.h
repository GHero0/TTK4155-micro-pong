#ifndef GLOBAL_H
#define GLOBAL_H
/*
 * global.h
 * This header file is included in every .c file of the project.
 * It is intended to contain extern declarations of global variables and shared definitions.
 * Actual global variable definitions should be in a globaldec.h
 */
#include "constants.h"

extern volatile char *sram;             // Start address for the SRAM
extern volatile char *adc;              // Start address for the ADC
extern unsigned int sram_address_space; // SRAM size 3KB
extern unsigned int adc_address_space;  // ADC size 1KB

// ADC interrupts
extern char Flag_ADC_ready;      // ADC data ready
// ADC channels
extern unsigned char adc_ch0;
extern unsigned char adc_ch1;
extern unsigned char adc_ch2;
extern unsigned char adc_ch3;

#endif
