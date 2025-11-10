/**
 * @file joystick.h
 * @brief Joystick function and global variables for position and direction
 */

#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "types.h"

void Joystick_Convert(void);
void Joystick_Init(void);

// Joystick Controls
extern JoyDir joystick_dir;
extern JoyPos joystick_pos;

extern unsigned char joy_x_max;
extern unsigned char joy_x_min;
extern unsigned char joy_y_max;
extern unsigned char joy_y_min;
extern unsigned char joy_x_center;
extern unsigned char joy_y_center;

#endif