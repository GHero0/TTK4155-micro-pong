#ifndef PWM_H
#define PWM_H

#include "types.h"

void PWM_Init(void);
void PWM_Update(unsigned char value);
uint8_t get_filtered_servo_value(int8_t new_value);

#endif