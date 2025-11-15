/**
 * @file main.h
 * @brief Includes header files that are needed and declares a main_init()
 */

#ifndef MAIN_H
#define MAIN_H

// =============================================================================
// INCLUDES
// =============================================================================

// Personal headers
#include "SRAM.h"
#include "inter.h"
#include "PWM.h"
#include "joystick.h"
#include "drivers/UART_driver.h"
#include "drivers/SPI.h"
#include "drivers/IO_board.h"
#include "drivers/OLED.h"
#include "sprites.h"
#include "images.h"
#include "UI.h"
#include "drivers/CAN.h"

// Libraries
#include <stdio.h>

// =============================================================================
// MAIN INIT
// =============================================================================

void Main_Init(void)
{
    PWM_Init();
    UART_Init();
    UART_Stdio_Init();
    SRAM_Init();
    Inter_Init();
    Joystick_Init();
    SPI_Init();
    OLED_Init();
    FrameBuffer_Init();
    CAN_Reset();
    CAN_Init();
    printf("===== SYSTEM INITIALIZED: NODE 1 =====\n");
}

#endif