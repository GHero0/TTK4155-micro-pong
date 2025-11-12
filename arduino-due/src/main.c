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
#include "drivers/motor_driver.h"
#include "PI_controller.h"


int main(void)
{
    SystemInit();
    PWM_Init();
    Timer_Init();  
    ADC_Init();
    motor_init();
    
    WDT->WDT_MR = WDT_MR_WDDIS;
    uart_init(84000000L, 9600);
    can_init_def_tx_rx_mb(CAN_BR_VALUE);
    
//     int speed = 35;	
//     int onTime = 200;
	
	// ##### controller common variables
	float pos_current = 0.0;
	float pos_ref = 0.0;
	//float kj = 5.0/1000.0;
	float kj = 1.5/1000.0;
	int8_t joystick_x = 0;
	int8_t joystick_y = 0;
	float controller_out = 0.0;
	
	// ############ PI - controller initialization
	PI_controller pi = {
// 		.kp = 12.0/1000.0,	
// 		.ki = 3.0/1000.0,		
// 		.kp = 12.0/1000.0,
// 		.ki = 11.0/1000.0,
		.kp = 25.0/1000.0,
		.ki = 0.0/1000.0,


		.t_sample = 1.0/10000.0,		
		.out_max = 100.0,
		.out_min = -100.0,		
		.term_i_max = 100.0,
		.term_i_min = -100.0		
	};
	float pi_controller_out = 0.0;
	PI_init(&pi);
	
	// ############ P - controller
	float kp  = 1.0/50.0;
	float error = 0.0 ;
	float p_controller_out = 0.0;

    printf("===== SYSTEM INITIALIZED =====\n");
    while (1)
    {
		// sample current position
		Update_Hand_Position();
		pos_current = (int) hand_position;
       
        // ##### store new value of Joystick position
        if (Flag_CAN_MB1)
        {
            if (mb1_buffer.id == 0)
            {
                joystick_x = (int8_t)mb1_buffer.data[0];  // integer part
				joystick_y = (int8_t)mb1_buffer.data[2];  // integer part
				
				//printf("error: %f;  controller_out: %f\n", error, controller_out);
				
                //uint8_t joystick_frac = mb1_buffer.data[1];       // fractional part
				
//                 uint8_t pwm_value = map_joystick_to_servo_fractional(joystick_int, joystick_frac);
//                 PWM_Update(pwm_value);
            }
            Flag_CAN_MB1 = 0;
        }
		
		// ######## setting pos_ref by the joystick value
		if (joystick_x > 5 || joystick_x < -5){
			pos_ref = pos_ref - kj * joystick_x;
		}		
		
		// ######## P-Controller
		error = pos_ref - (float)pos_current;
		p_controller_out = kp * error;
		
		// ######## PI-Controller
		pi_controller_out = PI_out(&pi, pos_ref, pos_current);
		
		// ######## pass Controller output to motor driver
		controller_out = pi_controller_out;		// use this to change controllers
		if (controller_out > 0){
			motor_move_left(controller_out);
		}
		if (controller_out < 0){
			motor_move_right(controller_out);
		}
		if (controller_out == 0){
			motor_stop();
		}

    }
}