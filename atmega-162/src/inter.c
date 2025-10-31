#include "inter.h"
#include "global.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>

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
    adc_ch0 = *adc;
    adc_ch1 = *adc;
    adc_ch2 = *adc;
    adc_ch3 = *adc;
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
