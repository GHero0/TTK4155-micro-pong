#ifndef GLOBAL_H
#define GLOBAL_H

/*
* global.h
* This header file is included in every .c file of the project.
* It is intended to contain extern declarations of global variables and shared definitions.
* Actual global variable definitions should be in a globaldec.h
*/

#include "constants.h"

extern volatile char Flag_CAN_MB1;
extern volatile char Flag_CAN_MB2;
extern volatile CAN_MESSAGE mb1_buffer;
extern volatile CAN_MESSAGE mb2_buffer;


#endif