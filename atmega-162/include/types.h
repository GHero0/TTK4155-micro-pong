#ifndef TYPES_H
#define TYPES_H

/*
* types.h
* Header file included when types are needed. 
* This file declares the types and structs needed
*/

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    int16_t X;  // Q8.8: –100.00 … +100.00
    int16_t Y;  // Q8.8: –100.00 … +100.00
} JOY_POS;

typedef enum
{
    UP,
    DOWN,
    RIGHT,
    LEFT,
    NEUTRAL,
} JOY_DIR;


typedef struct __attribute__((packed))
{
    union
    {
        uint8_t right;
        struct
        {
            uint8_t R1 : 1;
            uint8_t R2 : 1;
            uint8_t R3 : 1;
            uint8_t R4 : 1;
            uint8_t R5 : 1;
            uint8_t R6 : 1;
        };
    };
    union
    {
        uint8_t left;
        struct
        {
            uint8_t L1 : 1;
            uint8_t L2 : 1;
            uint8_t L3 : 1;
            uint8_t L4 : 1;
            uint8_t L5 : 1;
            uint8_t L6 : 1;
            uint8_t L7 : 1;
        };
    };
    union
    {
        uint8_t nav;
        struct
        {
            uint8_t NB : 1;
            uint8_t NR : 1;
            uint8_t ND : 1;
            uint8_t NL : 1;
            uint8_t NU : 1;
        };
    };
} BUTTONS;


typedef struct {
    unsigned char pwm;
    bool on;
} LED_STATE;

typedef struct {
    unsigned char x;
    unsigned char y;
    unsigned char size;
} TOUCH_PAD;

#endif 