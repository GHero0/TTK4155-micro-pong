#include "globaldec.h"
#include "drivers/UART_driver.h"
#include "SRAM.h"
#include "inter.h"
#include "PWM.h"
#include "joystick.h"
#include "tests/test_UART_driver.h"
#include "tests/test_latch.h"
#include "tests/test_SRAM.h"
#include "tests/test_inter.h"
#include "tests/test_adc.h"
#include "tests/test_joystick.h"
#include "drivers/SPI.h"
#include "drivers/OLED.h"

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
	
	// test_OLED();
	test_OLED();
	
    while (1)
    {
	    
    }
}
