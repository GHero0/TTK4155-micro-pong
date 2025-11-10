/**
 * @file joystick.c
 * @brief Joystick Handling and calibration calculus
 */

#include "joystick.h"
#include "types.h"
#include "config.h"
#include "SRAM.h"
#include "inter.h"

#include <stdlib.h>

// Joystick Controls
JoyDir joystick_dir = NEUTRAL;
JoyPos joystick_pos = {0, 0};
unsigned char joy_x_max = JOY_X_MAX;
unsigned char joy_x_min = JOY_X_MIN;
unsigned char joy_y_max = JOY_Y_MAX;
unsigned char joy_y_min = JOY_Y_MIN;
unsigned char joy_x_center = JOY_X_CENTER;
unsigned char joy_y_center = JOY_Y_CENTER;

// Joystick Calibration - separate scales for positive and negative ranges
unsigned int SCALE_X_POS = 0;  // For positive X (right)
unsigned int SCALE_X_NEG = 0;  // For negative X (left)
unsigned int SCALE_Y_POS = 0;  // For positive Y (up)
unsigned int SCALE_Y_NEG = 0;  // For negative Y (down)

void Joystick_Init(void)
{
    // Calculate scale factors for each direction from center
    // Positive direction (max from center)
    SCALE_X_POS = (100UL * 256UL) / (joy_x_max - joy_x_center);
    SCALE_Y_POS = (100UL * 256UL) / (joy_y_max - joy_y_center);
    
    // Negative direction (center from min)
    SCALE_X_NEG = (100UL * 256UL) / (joy_x_center - joy_x_min);
    SCALE_Y_NEG = (100UL * 256UL) / (joy_y_center - joy_y_min);
}

void Joystick_Convert(void)
{
    if (Flag_ADC_ready == 1)
    {
        // --- X axis (center = 0) ---
        if (adc_ch3 >= joy_x_center)
        {
            // Positive direction (right)
            int16_t dx = adc_ch3 - joy_x_center;
            if (adc_ch3 >= joy_x_max)
            {
                joystick_pos.X = 100 * 256;
            }
            else
            {
                joystick_pos.X = ((int16_t)dx * SCALE_X_POS);
            }
        }
        else
        {
            // Negative direction (left)
            int16_t dx = joy_x_center - adc_ch3;
            if (adc_ch3 <= joy_x_min)
            {
                joystick_pos.X = -100 * 256;
            }
            else
            {
                joystick_pos.X = -((int16_t)dx * SCALE_X_NEG);
            }
        }

        // --- Y axis (center = 0) ---
        if (adc_ch2 >= joy_y_center)
        {
            // Positive direction (up)
            int16_t dy = adc_ch2 - joy_y_center;
            if (adc_ch2 >= joy_y_max)
            {
                joystick_pos.Y = 100 * 256;
            }
            else
            {
                joystick_pos.Y = ((int16_t)dy * SCALE_Y_POS);
            }
        }
        else
        {
            // Negative direction (down)
            int16_t dy = joy_y_center - adc_ch2;
            if (adc_ch2 <= joy_y_min)
            {
                joystick_pos.Y = -100 * 256;
            }
            else
            {
                joystick_pos.Y = -((int16_t)dy * SCALE_Y_NEG);
            }
        }

        // --- Determine direction ---
        int16_t absX = abs(joystick_pos.X);
        int16_t absY = abs(joystick_pos.Y);
        
        if (absX < (JOY_DEADZONE_X << 8) && absY < (JOY_DEADZONE_Y << 8))
        {
            joystick_dir = NEUTRAL;
        }
        else if (absX > absY)
        {
            joystick_dir = (joystick_pos.X > 0) ? RIGHT : LEFT;
        }
        else
        {
            joystick_dir = (joystick_pos.Y > 0) ? UP : DOWN;
        }
        
        Flag_ADC_ready = 0;
    }
    *adc = 0x01; // Launch next conversion on ADC
}
