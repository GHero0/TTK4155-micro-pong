#ifndef CONSTANTS_H
#define CONSTANTS_H
/*
 * constants.h
 * This header file is included in global.h and globaldec.h.
 * It is meant for defining global constants, structs, enums, and constant values.
 */

#include <stdint.h>

#define F_CPU 5000000UL

typedef struct
{
    int16_t X;  // Q8.8: –100.00 … +100.00
    int16_t Y;  // Q8.8: –100.00 … +100.00
} JOY_POS;


typedef enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NEUTRAL,
} JOY_DIR;

#endif
