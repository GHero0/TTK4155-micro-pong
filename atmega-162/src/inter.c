/**
 * @file inter.c
 * @brief Interrupts init and Interrupt handling code along with
 * adc channels buffers and flag variables
 */

#include "inter.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "SRAM.h"

#include <stdio.h>

volatile char Flag_ADC_ready = 0;
volatile char Flag_screen = 0;
volatile char Flag_CAN = 0;

// ADC channels
unsigned char adc_ch0 = 0;
unsigned char adc_ch1 = 0;
unsigned char adc_ch2 = 0;
unsigned char adc_ch3 = 0;


void Inter_Init(void)
{
    // INT2 Disabled in General Interrupt Control
    GICR &= ~(1 << INT2);
    // Will interrupt on  rising edge
    EMCUCR |= (1 << ISC2);
    // Clear Interrupt Flag
    GIFR |= (1 << INTF2);
    // INT2 Enabled in General Interrupt Control
    GICR |= (1 << INT2);
	
	// INT0 Disabled in General Interrupt Control
	GICR &= ~(1 << INT0);
	// Will interrupt on  rising edge
	MCUCR |= (1 << ISC01);
	MCUCR &= ~(1 << ISC00);
	// Clear Interrupt Flag
	GIFR |= (1 << INTF0);
	// INT0 Enabled in General Interrupt Control
	GICR |= (1 << INT0);
	
    // Enable interrupts by applying the global interrupt mask
    sei();
}

ISR(INT2_vect)
{
    adc_ch0 = adc[0];
    adc_ch1 = adc[1];
    adc_ch2 = adc[2];
    adc_ch3 = adc[3];
    Flag_ADC_ready = 1;
}

ISR(TIMER0_COMP_vect)
{
    Flag_screen = 1;
}

ISR(INT0_vect)
{
	Flag_CAN = 1;
}
