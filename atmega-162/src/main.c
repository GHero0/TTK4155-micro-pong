#include "main.h"


messageCAN_t msgCAN = {
    .message_id = 2,
    .message_data_length = 4,
    .message_data = {0x11, 0x22, 0x33, 0x44}
};

int main(void)
{
    main_init();

    while (1)
    {
        if (Flag_screen)
        {
            FrameBufferClear();

            debug_window();

            FrameBufferPush();
            FrameBufferSwap();

            Flag_screen = 0;
        }

        if (Flag_CAN == 1)
        {
            // printf("\n\nINTERRUPT HERE\n\n");
            msgCAN = CAN_Receive_Message();
            // _delay_ms(1);
            // printf("====MSG====\n");
            // printf("id: 0x%X\n", msgCAN.message_id);
            // printf("data_length: 0x%X\n", msgCAN.message_data_length);

            // for (uint8_t i = 0; i < msgCAN.message_data_length; i++)
            // {
            //     printf("data[%d]: 0x%2X\n", i, msgCAN.message_data[i]);
            // }
            Flag_CAN = 0;
        }

        CAN_Send_Message(msgCAN);

        IO_board_update();
        Joystick_convert();

        map_touchpad();
    }
}