#ifndef IO_BOARD_H
#define IO_BOARD_H

#include "types.h"

#define LED_COUNT 6

void IO_board_update(void);

// IO-Board Buttons
extern BUTTONS buttons;
// Maybe change later on ... ! 
extern TOUCH_PAD touch_pad;
extern unsigned char slider;  
extern LED_STATE leds[6];

#endif