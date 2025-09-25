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

// Joystick Controls
JOY_DIR joystick_dir = NEUTRAL;
JOY_POS joystick_pos = {0, 0};
// Joystick Calibration
unsigned char JOY_X_MIN = 68;
unsigned char JOY_X_MAX = 248;
unsigned char JOY_Y_MIN = 68;
unsigned char JOY_Y_MAX = 248;
unsigned char JOY_X_CENTER = 164;
unsigned char JOY_Y_CENTER = 164;
unsigned int SCALE_X = 0;
unsigned int SCALE_Y = 0;
unsigned char DEADZONE_X = 10;
unsigned char DEADZONE_Y = 10;

// IO-Board 
BUTTONS buttons = {0,0,0};
// Maybe change later on ... ! 
unsigned char slider = 0;  
unsigned char touch_x = 0; 
unsigned char touch_y = 0; 
unsigned char touch_size = 0;
unsigned char leds = 0;

#endif
