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

// Joystick Calibration
unsigned int SCALE_X = 0;
unsigned int SCALE_Y = 0;

// single SCALE used for both axes (if ranges differ, define SCALE_X and SCALE_Y)
void Joystick_Init(void)
{
    SCALE_X = (200UL * 256UL) / (JOY_X_MAX - JOY_X_MIN);
    SCALE_Y = (200UL * 256UL) / (JOY_Y_MAX - JOY_Y_MIN);
}

void Joystick_Convert(void)
{
    if (Flag_ADC_ready == 1)
    {

        // --- X axis ---
        int16_t dx = adc_ch2 - JOY_X_MIN;

        if (adc_ch2 <= JOY_X_MIN)
        {
            joystick_pos.X = -100 * 256;
        }
        else if (adc_ch2 >= JOY_X_MAX)
        {
            joystick_pos.X = 100 * 256;
        }
        else
        {
            joystick_pos.X = ((int32_t)dx * SCALE_X) - (100 * 256);
        }

        // --- Y axis ---
        int16_t dy = adc_ch3 - JOY_Y_MIN;

        if (adc_ch3 <= JOY_Y_MIN)
        {
            joystick_pos.Y = -100 * 256;
        }
        else if (adc_ch3 >= JOY_Y_MAX)
        {
            joystick_pos.Y = 100 * 256;
        }
        else
        {
            joystick_pos.Y = ((int32_t)dy * SCALE_Y) - (100 * 256);
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
