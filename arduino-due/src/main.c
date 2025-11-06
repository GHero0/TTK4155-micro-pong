#include "types.h"
#include "sam.h"
#include <stdio.h>
#include "time.h"
#include "drivers/uart.h"
#include "drivers/can_controller.h"
#include "drivers/can_interrupt.h"
#include "PWM.h"
#include "ADC.h"
#include "timer.h"

int main(void)
{
    SystemInit();
    PWM_Init();
    Timer_Init();
    ADC_Init();
    WDT->WDT_MR = WDT_MR_WDDIS; // Disable Watchdog Timer

    uart_init(84000000L, 9600);
    can_init_def_tx_rx_mb(CAN_BR_VALUE);

    while (1)
    {
        // // For now no filter on MB1/MB2 so both receive the same message
        if (Flag_CAN_MB1)
        {
            // printf("CAN Message received:\n");
            // printf("  ID: \t\t0x%03X\n", mb1_buffer.id);
            // printf("  Length: \t%d\n", mb1_buffer.data_length);
            // printf("  Data: \t");
            // for (uint8_t i = 0; i < mb1_buffer.data_length; i++)
            // printf("%d ", (signed char)mb1_buffer.data[i]);
            // printf("\n");
            

            if (mb1_buffer.id == 0){
                PWM_Update(100 - (((signed char)mb1_buffer.data[0]+100)/2));               
            }
            Flag_CAN_MB1 = 0;
        }
        if (Flag_ADC)
        {   
            // Convert to voltage for debugging
            float voltage = (adc11_result * 3.3f) / 4095.0f;
            printf("ADC: 0x%03X (%4d) = %.3fV\n", adc11_result, adc11_result, voltage);
            Flag_ADC = 0;
        }
        // printf("Something, Arduino is allow to do whatever ! \n\n");
        // time_spinFor(msecs(1000));
    }
}