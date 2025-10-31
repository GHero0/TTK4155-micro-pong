#include "tests/test_UI.h"
#include "global.h"
#include "sprites.h"
#include "UI.h"

// ! Carefull about these values, as they are declared at global state
// ! and might conflict with other variables 
int X = 32;
int Y = -64;
unsigned char step_anim = 0; 
static unsigned char initialized = 0;


void debug_window(void)
{
    // Initialize on first call
    if (!initialized)
    {
        Y_window_1 = -64;
        Y_window_2 = -64;
        X_window_1 = 8;
        X_window_2 = 72;
        initialized = 1;
    }

    draw_line(127, 0, X, Y);
    if (step_anim < 10)
    {
        step_anim++;
    }
    else
    {
        step_anim = 0;
    }
    if (Y_window_1 < 8)
    {
        Y_window_1 += 2;
    }
    if (Y_window_2 < 16)
    {
        Y_window_2 += 2;
    }
    draw_window(X_window_2, Y_window_2, 6, 3);
    draw_window(X_window_1, Y_window_1, 7, 4);

    draw_printf(X_window_1 + 3, Y_window_1 + 16, "X:%d\nY:%d", joystick_pos.X >> 8, joystick_pos.Y >> 8);
    draw_printf(X_window_1 + 27, Y_window_1 + 16,"X':%d\nY':%d\nS:%d", X,Y,touch_pad.size);
    draw_task_bar();

    joystick_indicator(X_window_2 + 8, Y_window_2 + 16, 0);
    joystick_indicator(X_window_2 + 32, Y_window_2 + 16, 1);
    cursor();
}

void map_touchpad(void)
{
    // Map touchpad
    X = touch_pad.x >> 1;
    Y = 63 - (touch_pad.y >> 2);

    if (X < 0)
        X = 0;
    if (X > 127)
        X = 127;
    if (Y < 0)
        Y = 0;
    if (Y > 63)
        Y = 63;
}