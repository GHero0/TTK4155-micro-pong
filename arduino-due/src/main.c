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
#include "drivers/motor_driver.h"
#include "PI_controller.h"
#include "solenoid.h"
#include "can_messages.h"

#include <stdio.h>

int main(void)
{
    // System Initialization
    SystemInit();
    PWM_Init();
    Timer_Init();
    ADC_Init();
    motor_init();
    Solenoid_Init();
    PI_init(&pi);    
    uart_init(84000000L, 9600);
    can_init_def_tx_rx_mb(CAN_BR_VALUE);
    
    // Disable Watch Dog
    WDT->WDT_MR = WDT_MR_WDDIS;

    printf("===== SYSTEM INITIALIZED =====\n");

    while (1)
    {
        pos_current = get_safe_position();
        handle_can_messages();        
        handle_scoring_system();

        if (joystick_x > JOYSTICK_DEADZONE || joystick_x < -JOYSTICK_DEADZONE) {
            pos_ref -= JOYSTICK_GAIN * joystick_x;
            pos_ref = clamp_float(pos_ref, POSITION_MIN, POSITION_MAX);  // ← FIX!
        }

        float controller_out = PI_out(&pi, pos_ref, pos_current);
        update_motor(controller_out);
    }
}