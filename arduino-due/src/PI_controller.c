/*
 * PI_controller.c
 *
 * Created: 12.11.2025 11:49:39
 *  Author: flaucht
 */ 


#include "PI_controller.h"
#include "types.h"
#include "types.h"
#include "config.h"
#include "drivers/motor_driver.h"
#include "encoder.h"

// Control Variables
float pos_current = 0.0f;
float pos_ref = 0.0f;

PI_controller pi = {
    .kp = PI_KP,
    .ki = PI_KI,
    .t_sample = PI_SAMPLE_TIME,
    .out_max = PI_OUTPUT_MAX,
    .out_min = PI_OUTPUT_MIN,
    .term_i_max = PI_OUTPUT_MAX,
    .term_i_min = PI_OUTPUT_MIN
};


void PI_init(PI_controller *pi)
{
    pi->term_i = 0.0f;
    pi->term_p = 0.0f;
    pi->error_prev = 0.0f;
    pi->out = 0.0f;
}

float PI_out(PI_controller *pi, float ref_value, float pos_current)
{
    float error = ref_value - pos_current;
    
    pi->term_p = pi->kp * error;
    pi->term_i += 0.5f * pi->ki * pi->t_sample * (error + pi->error_prev);
    
    // Anti-windup: clamp integral term
    if (pi->term_i > pi->term_i_max) {
        pi->term_i = pi->term_i_max;
    }
    else if (pi->term_i < pi->term_i_min) {
        pi->term_i = pi->term_i_min;
    }
    
    // Compute output
    pi->out = pi->term_p + pi->term_i;
    
    // Clamp output to limits
    if (pi->out > pi->out_max) {
        pi->out = pi->out_max;
    }
    else if (pi->out < pi->out_min) {
        pi->out = pi->out_min;
    }
    
    // Store error for next iteration
    pi->error_prev = error;
    
    return pi->out;
}

void update_motor(float controller_output)
{
    if (controller_output > MOTOR_DEADZONE) {
        motor_move_left(controller_output);
    } 
    else if (controller_output < -MOTOR_DEADZONE) {
        motor_move_right(-controller_output);
    } 
    else {
        motor_stop();
    }
}

float get_safe_position(void)
{
    Update_Hand_Position();
    
    if (hand_position > 1000000) {  // Wraparound
        hand_position = 0;
        return 0.0f;
    }
    
    if (hand_position > POSITION_OVERSHOOT) {  // Overshoot
        hand_position = (uint32_t)POSITION_MAX;
        return POSITION_MAX;
    }
    
    return (float)hand_position;
}

// ========== Helper Functions ==========
float clamp_float(float value, float min, float max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}
