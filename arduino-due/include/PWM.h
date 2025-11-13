#ifndef PWM_H
#define PWM_H

#include "types.h"

void PWM_Init(void);
void PWM_Update(uint8_t value);
void PWM_Motor(int fraction);        // update DC motor PWM (0..100)

#endif
