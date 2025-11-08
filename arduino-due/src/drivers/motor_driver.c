/*
 * motor_driver.c
 *
 * Created: 08.11.2025 11:09:14
 *  Author: flaucht
 */ 

#include "sam.h"
#include <stdio.h>
#include "PWM.h"
#include "time.h"

// PWM_init function also has to be called for motor to be initialized work
void motor_init(){
	PMC->PMC_PCER0 |= (1 << ID_PIOC);		// enable peripheral for direction pin
	PIOC->PIO_OER = PIO_OER_P23;			// set direction pin as output
}


// lets motor move at a desired speed to a desired direction
// speed [0,100]
// dir 0: left, dir 1 right
void motor_move(int speed, int dir){
	if (dir == 1){
		PIOC->PIO_SODR = PIO_SODR_P23;	// direction = 1 to the right
	}
	else if (dir == 0){
		PIOC->PIO_CODR = PIO_SODR_P23;	// direction = 0 to the left
	}	
	
	if(speed > 100){
		speed = 100;
	}
	else if (speed < 0){
		speed = 0;
	}	
	PWM_Motor(speed);
}

void motor_move_left(int speed){
	motor_move(speed, 0);
}

void motor_move_right(int speed){
	motor_move(speed, 1);
}

// lets motor move at a desired speed to a desired direction for a certain amount of time
// speed [0,100]
// dir 0: left, dir 1 right
// onTime in milli sec
void motor_move_for_time(int speed, int dir, int onTime){
	void motor_move(speed,dir);
	time_spinFor(msecs(onTime));
	PWM_Motor(0);
}

void motor_stop(void){
	PWM_Motor(0);
}