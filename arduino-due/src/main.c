#include "types.h"
#include "sam.h"
#include "time.h"
#include "drivers/uart.h"
#include "drivers/can_controller.h"
#include "drivers/can_interrupt.h"
#include "PWM.h"
#include "ADC.h"
#include "timer.h"
#include "config.h"
#include "encoder.h"

#include <stdio.h>

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
                // Filter the joystick input before sending to servo
                int8_t joystick_value = (signed char)mb1_buffer.data[0];
                uint8_t filtered_pwm = get_filtered_servo_value(joystick_value);
                PWM_Update(filtered_pwm);  
            }
            Flag_CAN_MB1 = 0;
        }

        handle_scoring_system();
        Update_Hand_Position();
        printf("Encoder Position: %lu\n", hand_position);
    }
}