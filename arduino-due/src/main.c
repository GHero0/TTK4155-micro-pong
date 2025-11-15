/**
 * @file main.c
 * @brief Main function with light code
 */

// =============================================================================
// INCLUDES
// =============================================================================

// Personal headers
#include "main.h"

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main(void)
{
    Main_Init();
    while (1)
    {
        handle_can_messages();  
        handle_scoring_system();

        // PI controller runs ONLY at fixed 50Hz
        if (Flag_PI_update) 
        {
            Flag_PI_update = 0;            
            pos_current = get_safe_position();
            
            // Update reference from joystick
            if (joystick_x > JOYSTICK_DEADZONE || joystick_x < -JOYSTICK_DEADZONE) 
            {
                // Joystick map mode
                if (game_mode == 1){
                    pos_ref = joystick_x * (-28.0f) + 2800.0f;
                    pos_ref = clamp_float(pos_ref, POSITION_MIN, POSITION_MAX);
                }

                // Relative map mode
                if (game_mode == 2){
                    pos_ref -= JOYSTICK_GAIN * joystick_x;
                    pos_ref = clamp_float(pos_ref, POSITION_MIN, POSITION_MAX);
                }

            }
            
            float controller_out = PI_out(&pi, pos_ref, pos_current);    
            update_motor(controller_out);     
        }
    }
}