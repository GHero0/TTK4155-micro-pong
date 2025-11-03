#include "tests/test_joystick.h"
#include "joystick.h"
#include "inter.h"
#include <stdio.h>

void test_joystick(void)
{
    Joystick_convert();

    printf("X: %4d.%03d, Y: %4d.%03d, CH02: %3d, CH03: %3d, Dir: %5s\n",
           joystick_pos.X >> 8, joystick_pos.X & 0xFF,
           joystick_pos.Y >> 8, joystick_pos.Y & 0xFF,
           adc_ch2, adc_ch3,
           joystick_dir == UP ? "UP" : joystick_dir == DOWN   ? "DOWN"
                                     : joystick_dir == LEFT   ? "LEFT"
                                     : joystick_dir == RIGHT  ? "RIGHT"
                                                              : "NEUTRAL");
}