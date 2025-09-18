#include "PWM.h"
#include "global.h"

#include <avr/io.h>
#include <stdio.h>


void PWM_init(void){
	// setting OC2 (PB1) pin as output
	DDRB = 1<<PB1;
	
	// setting up FOC bit
	TCCR2 = 0;
		
	// setting up Wave Generator pins 
	// TCCR2 = TCCR2 | ( (1<<WGM21) | (1<<WGM20) );		// for fast PWM mode
	TCCR2 |= (1<<WGM20);							// for phase correct PWM mode 
	
	// setting up COM bits for noninverting PWM/ 50% duty cycle
	// TCCR2 = TCCR2 | (1<<COM21);			//  COM21:0 = 2, noninverting PWM, values in OCR2 have to be set
	TCCR2 |= (1<<COM21);			//  Clear OC2 on Compare Match, set OC2 at TOP
		
	// setting up Prescaler 
	//TCCR2 = TCCR2 | (1<<CS21);		// to N=8	CS22, CS21, CS20 = 0, 1, 0 
	TCCR2 |= (1<<CS20);		// to N=1   CS22, CS21, CS20 = 0, 0, 1
	
	// setting up OCR2 register for controlling duty cycle
	// OCR2 = 0b10000000;	// MAX value/2 -> should result in 50% duty cycle, works 
	OCR2 = 230;				// 90% duty cycle, 0,9*2^8
		
}

