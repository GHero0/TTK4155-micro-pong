// =============================================================================
// INCLUDES
// =============================================================================

// Personal headers
#include "can_messages.h"
#include "types.h"
#include "drivers/can_interrupt.h"
#include "PWM.h"
#include "solenoid.h"
#include "can_messages.h"

#include <stdio.h>

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

int8_t joystick_x = 0;
int8_t joystick_y = 0;
int8_t game_mode = 0;
int8_t prev_game_mode = 0;
CANMessage mb0_buffer = {0};

// =============================================================================
// CAN MESSAGE HANDLER
// =============================================================================

void handle_can_messages(void)
{
    if (!Flag_CAN_MB1) return;

    if (mb1_buffer.id == 10) {
        // Update joystick values
        joystick_x = (int8_t)mb1_buffer.data[0];
        joystick_y = (int8_t)mb1_buffer.data[2];
        
        // Update servo with Y-axis
        int8_t joystick_int   = (int8_t)mb1_buffer.data[2];
        uint8_t joystick_frac = mb1_buffer.data[3];           
        uint8_t pwm_value     = map_joystick_to_servo_fractional(joystick_int, joystick_frac);
        
        PWM_Update(pwm_value);
    }
    
    if (mb1_buffer.id == 7) {
        prev_game_mode = game_mode;
        game_mode = mb1_buffer.data[0];
    }

    if (mb1_buffer.id == 9) {
        Solenoid_Update(mb1_buffer.data[0]);
    }
    
    Flag_CAN_MB1 = 0;
}


int clamp_int(int val, int min, int max)
{
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

uint8_t map_joystick_to_servo_fractional(int8_t joystick_int, uint8_t joystick_frac)
{
    joystick_int = clamp_int(joystick_int, -100, 100);
    
    int32_t value_16bit = ((int32_t)joystick_int << 8) + joystick_frac;
    
    uint32_t temp = (uint32_t)(100*256 - (value_16bit + 100*256) / 2);
    
    return (uint8_t)((temp + 128) / 256);
}
