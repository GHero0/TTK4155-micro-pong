#include "PWM.h"
#include "global.h"

#include <avr/io.h>
#include <stdio.h>

void PWM_init(void)
{
    DDRD |= (1 << PD5);
    TCCR1A = 0;
    TCCR1B = 0;


    TCCR1B |= (1 << WGM12)|(1<<WGM10);

    TCCR1A |= (1 << COM1A0);

    TCCR1B |= (1 << CS10);

    OCR1A = 1; // 50% duty cycle
}
