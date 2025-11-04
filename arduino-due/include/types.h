/**
 * @file types.h
 * @brief Common type definitions used throughout the project
 * 
 * All structs, enums, and typedefs in one place.
 * No extern declarations or function prototypes here.
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

// =============================================================================
// CAN TYPES
// =============================================================================

/**
 * @brief CAN message structure
 */
typedef struct can_message_t
{
	uint16_t id;
	char data_length;
	char data[8];
} CANMessage;

#endif