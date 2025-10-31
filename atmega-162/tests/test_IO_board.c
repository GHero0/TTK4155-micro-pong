#include "tests/test_IO_board.h"
#include "drivers/IO_board.h"
#include "global.h"

#include <util/delay.h>
#include <stdio.h>

const unsigned char wave_table[WAVE_STEPS] = {
    0, 16, 32, 48, 64, 80, 96, 112,
    128, 144, 160, 176, 192, 208, 224, 240,
    224, 208, 192, 176, 160, 144, 128, 112,
    96, 80, 64, 48, 32, 16, 0, 0};

unsigned char step = 0;

void leds_wave_animation_split(unsigned char step)
{
    unsigned char delay = 1; // phase delay between LEDs

    // Left-to-right wave (LEDs 0,1,2)
    for (int i = 0; i <= 2; i++)
    {
        unsigned char index = (step + i * delay) % WAVE_STEPS;
        leds[i].pwm = wave_table[index];
    }

    // Right-to-left wave (LEDs 3,4,5)
    for (int i = 3; i <= 5; i++)
    {
        unsigned char reverse_i = 5 - i; // 3->2, 4->1, 5->0
        unsigned char index = (step + reverse_i * delay) % WAVE_STEPS;
        leds[i].pwm = wave_table[index];
    }
}

void test_IO_board(void)
{
    IO_board_update();
    leds_wave_animation_split(step);
    step = (step + 1) % WAVE_STEPS;
    
    printf("\033[2J\033[H%02X %02X %02X %3d %3d %3d %3d\n",
           buttons.right, buttons.left, buttons.nav, 
           slider, touch_pad.x, touch_pad.y, touch_pad.size);
    printf("L^ R^ N^ SL^ TX^ TY^ TS^");
    _delay_ms(3); // Important delay to allow buffer flush
}