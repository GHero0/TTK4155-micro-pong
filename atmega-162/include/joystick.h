#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "types.h"

void Joystick_convert(void);
void Joystick_init(void);

// Joystick Controls
extern JOY_DIR joystick_dir;
extern JOY_POS joystick_pos;
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