#include "types.h"
#include "sam.h"
#include <stdio.h>
#include "time.h"
#include "drivers/uart.h"
#include "drivers/can_controller.h"
#include "drivers/can_interrupt.h"
#include "PWM.h"
#include "ADC.h"
#include "timer.h"

#define BALL_DETECT_THRESHOLD 1000    
#define BALL_CLEAR_THRESHOLD  3700    

uint8_t score = 0;
bool timer_started = false;
bool point_was_scored = false;

int main(void)
{
    SystemInit();
    PWM_Init();
    Timer_Init();  
    ADC_Init();
    WDT->WDT_MR = WDT_MR_WDDIS;
    uart_init(84000000L, 9600);
    can_init_def_tx_rx_mb(CAN_BR_VALUE);
    
    printf("===== SYSTEM INITIALIZED =====\n");
    while (1)
    {
        if (Flag_CAN_MB1)
        {
            if (mb1_buffer.id == 0){
                PWM_Update(100 - (((signed char)mb1_buffer.data[0]+100)/2));               
            }
            Flag_CAN_MB1 = 0;
        }
        
        if (Flag_ADC)
        {               
            if (adc11_result < BALL_DETECT_THRESHOLD)  
            {
                if (!timer_started && !point_was_scored)
                {   
                    Counter_Lose_Score_Start();  
                    timer_started = true;
                }                
            }
            else if (adc11_result > BALL_CLEAR_THRESHOLD)  
            {
                if (timer_started)
                {   
                    Counter_Lose_Score_Stop();  
                    timer_started = false;
                }
                
                if (point_was_scored)
                {
                    point_was_scored = false;
                }
            }
            
            Flag_ADC = 0;
        }
        
        if (Flag_Point_Lose)
        {
            if (!point_was_scored)
            {
                score++;
                printf("POINT SCORED! Total: %d\n", score);
                point_was_scored = true;
            }
            
            timer_started = false;  
            Flag_Point_Lose = 0;
        }
    }
}