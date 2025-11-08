#ifndef PWM_H
#define PWM_H

void PWM_Init(void);
void PWM_Update(unsigned char value);
void PWM_Motor(int fraction);

#endif