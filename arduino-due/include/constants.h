#ifndef CONSTANTS_H
#define CONSTANTS_H

/*
 * constants.h
 * This header file is included in global.h and globaldec.h.
 * It is meant for defining global constants, structs, enums, and constant values.
 */
#include <stdint.h>

typedef struct can_message_t
{
	uint16_t id;
	char data_length;
	char data[8];
} CAN_MESSAGE;

#endif