/**
 * @file PWM.c
 * @brief PWM_init() function that enables timers
 */

#include "PWM.h"

#include <avr/io.h>

void PWM_Init(void)
{
    DDRD |= (1 << PD5);
    TCCR1A = 0;
    TCCR1B = 0;

    // -- Timer1: 2.5MHz ---
    TCCR1B |= (1 << WGM12) | (1 << WGM10);
    TCCR1A |= (1 << COM1A0);
    TCCR1B |= (1 << CS10);
    OCR1A = 1; // 50% duty cycle

    
    // --- Timer0: 1 ms tick ---
    TCCR0 |= (1 << WGM01);
    TCCR0 |= (1 << CS01) | (1 << CS00);
    OCR0 = 77;

    TIMSK |= (1 << OCIE0);
}
