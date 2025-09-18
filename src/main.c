#include "globaldec.h"
#include "drivers/UART_driver.h"
#include "SRAM.h"
#include "inter.h"
#include "tests/test_UART_driver.h"
#include "tests/test_latch.h"
#include "tests/test_SRAM.h"
#include "PWM.h"
#include "tests/test_inter2.h"


int main(void)
{
	PWM_init();
	USART_Init();
	UART_stdio_init();
	SRAM_Init();
	Inter_Init();

	while (1)
	{
		test_inter2();
	}
}
