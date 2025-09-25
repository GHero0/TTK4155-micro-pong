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

// Joystick Controls
extern JOY_DIR joystick_dir;
extern JOY_POS joystick_pos;
// Joystick Calibration
extern unsigned char JOY_X_MIN;
extern unsigned char JOY_X_MAX;
extern unsigned char JOY_Y_MIN;
extern unsigned char JOY_Y_MAX;
extern unsigned char JOY_X_CENTER;
extern unsigned char JOY_Y_CENTER;
extern unsigned int SCALE_X;
extern unsigned int SCALE_Y;
extern unsigned char DEADZONE_X;
extern unsigned char DEADZONE_Y; 

#endif
