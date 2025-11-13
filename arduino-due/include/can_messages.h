#ifndef CAN_MESSAGES_H
#define CAN_MESSAGES_H

#include "types.h"

uint8_t map_joystick_to_servo_fractional(int8_t joystick_int, uint8_t joystick_frac);
void handle_can_messages(void);
int clamp_int(int val, int min, int max);

extern int8_t joystick_x;
extern int8_t joystick_y;

#endif