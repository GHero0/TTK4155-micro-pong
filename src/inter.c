#include "inter.h"
#include <avr/io.h>
#include <avr/interrupt.h>

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

    // Enable interrupts by applying the global interrupt mask
    sei();
}

