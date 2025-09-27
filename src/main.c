#include "globaldec.h"
#include "SRAM.h"
#include "inter.h"
#include "PWM.h"
#include "joystick.h"
#include "drivers/UART_driver.h"
#include "drivers/SPI.h"
#include "drivers/OLED.h"

#include "tests/test_OLED.h"

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdlib.h>

int main(void)
{
    PWM_init();
    USART_Init();
    UART_stdio_init();
    SRAM_Init();
    Inter_Init();
    Joystick_init();
    OLED_Init();
	
    OLED_FillScreen(0x00); // Clear screen
    while (1)
    {
        test_OLED();
    }
}
