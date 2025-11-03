#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "types.h"

void Joystick_Convert(void);
void Joystick_Init(void);

// Joystick Controls
extern JoyDir joystick_dir;
extern JoyPos joystick_pos;
// Joystick Calibration
extern unsigned char JOY_X_MIN;
extern unsigned char JOY_X_MAX;
extern unsigned char JOY_Y_MIN;
extern unsigned char JOY_Y_MAX;
extern unsigned char JOY_X_CENTER;
extern unsigned char JOY_Y_CENTER;
extern unsigned int SCALE_X;
extern unsigned int SCALE_Y;
extern unsigned char DEADZONE_X;
extern unsigned char DEADZONE_Y; 

#endif