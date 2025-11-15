#ifndef CAN_MESSAGES_H
#define CAN_MESSAGES_H

// =============================================================================
// INCLUDES
// =============================================================================

// Personal headers
#include "types.h"

// =============================================================================
// CAN MESSAGE HANDLER FUNCTIONS
// =============================================================================

uint8_t map_joystick_to_servo_fractional(int8_t joystick_int, uint8_t joystick_frac);
void handle_can_messages(void);
int clamp_int(int val, int min, int max);

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================



extern int8_t joystick_x;
extern int8_t joystick_y;
extern int8_t game_mode;
extern int8_t prev_game_mode;


#endif