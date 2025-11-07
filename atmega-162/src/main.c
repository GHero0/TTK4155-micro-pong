/**
 * @file main.c
 * @brief Main project loop logic implementation
 */

#include "main.h"

#include <stdio.h>
#include <util/delay.h>

int main(void)
{
    Main_Init();

    while (1)
    {
        if (Flag_screen)
        {
            FrameBuffer_Clear();

            display_current_screen();
            
            FrameBuffer_Push();
            FrameBuffer_Swap();

            Flag_screen = 0;
        }

        if (current_screen == SCREEN_DEBUG_BLUE_BOX){

            msgCAN_TX.message_id = 0;
            msgCAN_TX.message_data_length = 4;
            msgCAN_TX.message_data[0] = joystick_pos.X >> 8;
            msgCAN_TX.message_data[1] = joystick_pos.X;
            msgCAN_TX.message_data[2] = joystick_pos.Y >> 8;
            msgCAN_TX.message_data[3] = joystick_pos.Y;    

            CAN_Send_Message(msgCAN_TX);
        }

        IO_board_update();
        Joystick_Convert();

        map_touchpad();
    }
}