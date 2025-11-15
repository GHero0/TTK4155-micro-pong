/**
 * @file main.c
 * @brief Main project loop logic implementation
 */

// =============================================================================
// INCLUDES
// =============================================================================

// Personal headers
#include "main.h"

// Libraries
#include <stdio.h>
#include <util/delay.h>

// =============================================================================
// MAIN FUNCTION
// =============================================================================

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

            // Score 
            if (Flag_CAN == 1) {
                msgCAN_RX = CAN_Receive_Message();
                if (msgCAN_RX.message_id == 8) {
                    score = msgCAN_RX.message_data[0];
                }
                Flag_CAN = 0;
            }

            // Left Joystick 
            msgCAN_TX.message_id = 10;
            msgCAN_TX.message_data_length = 4;
            msgCAN_TX.message_data[0] = joystick_pos.X >> 8;
            msgCAN_TX.message_data[1] = joystick_pos.X;
            msgCAN_TX.message_data[2] = joystick_pos.Y >> 8;
            msgCAN_TX.message_data[3] = joystick_pos.Y;    


            CAN_Send_Message(msgCAN_TX);

            // Push Button
            msgCAN_TX.message_id = 9;
            msgCAN_TX.message_data_length = 1;
            msgCAN_TX.message_data[0] = buttons.R5; 

            CAN_Send_Message(msgCAN_TX);
        }

        IO_board_update();
        Joystick_Convert();
    }
}