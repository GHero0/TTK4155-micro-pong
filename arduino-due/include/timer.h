#ifndef TIMER_H
#define TIMER_H

#include "types.h"

void Timer_Init(void);
void Counter_Lose_Score_Start(void);
void Counter_Lose_Score_Restart(void);
void Counter_Lose_Score_Stop(void);

extern volatile uint8_t Flag_Point_Lose;
extern uint32_t hand_position;



#endif