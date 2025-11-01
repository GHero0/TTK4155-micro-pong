#include "globaldec.h"
#include "sam.h"
#include <stdio.h>
#include "time.h"
#include "drivers/uart.h"
#include "drivers/can_controller.h"
#include "drivers/can_interrupt.h"

int main(void)
{
    SystemInit();
    WDT->WDT_MR = WDT_MR_WDDIS; // Disable Watchdog Timer

    uart_init(84000000L, 9600);
    can_init_def_tx_rx_mb(CAN_BR_VALUE);

    while (1)
    {
        // For now no filter on MB1/MB2 so both receive the same message
        if (Flag_CAN_MB1)
        {
            printf("CAN Message received:\n");
            printf("  ID: \t\t0x%03X\n", mb1_buffer.id);
            printf("  Length: \t%d\n", mb1_buffer.data_length);
            printf("  Data: \t");
            for (uint8_t i = 0; i < mb1_buffer.data_length; i++)
            printf("0x%02X ", mb1_buffer.data[i]);
            printf("\n");
            Flag_CAN_MB1 = 0;
        }
        printf("Something, Arduino is allow to do whatever ! \n\n");
        time_spinFor(msecs(1000));
    }
}