#ifndef MAIN_H
#define MAIN_H

// =============================================================================
// INCLUDES
// =============================================================================

// Personal headers
#include "types.h"
#include "drivers/uart.h"
#include "drivers/can_controller.h"
#include "drivers/can_interrupt.h"
#include "PWM.h"
#include "ADC.h"
#include "timer.h"
#include "config.h"
#include "encoder.h"
#include "drivers/motor_driver.h"
#include "PI_controller.h"
#include "solenoid.h"
#include "can_messages.h"

// Libraries
#include <time.h>
#include <sam.h>
#include <stdio.h>

// =============================================================================
// MAIN INIT
// =============================================================================

void Main_Init(void)
{
    // System Initialization
    SystemInit();
    PWM_Init();
    Timer_Init();
    ADC_Init();
    motor_init();
    Solenoid_Init();
    uart_init(84000000L, 9600);
    can_init_def_tx_rx_mb(CAN_BR_VALUE);
    PI_init(&pi);    
    
    // Disable Watch Dog
    WDT->WDT_MR = WDT_MR_WDDIS;

    PWM_Update(50);
    printf("===== SYSTEM INITIALIZED: NODE 2 =====\n");
}

#endif