#ifndef IO_BOARD_H
#define IO_BOARD_H

#include "types.h"

void IO_board_update(void);

// IO-Board Buttons
extern Buttons buttons;
// Maybe change later on ... ! 
extern TouchPad touch_pad;
extern unsigned char slider;  
extern LedState leds[6];

#endif