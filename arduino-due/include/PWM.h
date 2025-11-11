#ifndef PWM_H
#define PWM_H

#include "types.h"

void PWM_Init(void);
void PWM_Update(uint8_t value);
void PWM_Motor(int fraction);        // update DC motor PWM (0..100)
uint8_t map_joystick_to_servo_fractional(int8_t joystick_int, uint8_t joystick_frac);

#endif
