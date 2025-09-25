#include "drivers/SPI.h"
#include "drivers/OLED.h"
#include "global.h"
#include <stdio.h>

#include "util/delay.h"

// To erase the noise/residues on the screen
void test_function_Clear_Screen_From_Noise(void)
{ 
	for (uint8_t p = 0; p < 8; ++p) 
	{
		OLED_FillScreen(p, 0x00);
	}
}

void test_OLED()
{
	// Initialize the OLED device
	OLED_Init();
	
	// Clear the screen
	test_function_Clear_Screen_From_Noise();
	
	// Run the test code
	OLED_FillScreen(0, 0xFF);
	_delay_ms(500);
	OLED_FillScreen(0, 0x00);
	_delay_ms(500);
	
	while(1)
	{		
		// Algorithm : TEST
		OLED_Home();
		OLED_Go_To_Pos(0, 20);
		OLED_Print("Hello guys!\n");
		_delay_ms(400);
		OLED_Print("We are\n");
		_delay_ms(400);
		OLED_Go_To_Pos(2, 20+30);
		OLED_Print("here\n");
		_delay_ms(400);
		OLED_Go_To_Pos(3, 20+60);
		OLED_Print("to play\n");
		_delay_ms(400);
		OLED_Go_To_Pos(5, 0);
		OLED_Print("ping-pong.\n");
		_delay_ms(1000);
		
		OLED_Clear_Screen();
		
		OLED_Home();
		OLED_Print("OLED reset");
		_delay_ms(300);
		OLED_Print(".");
		_delay_ms(300);
		OLED_Print(".");
		_delay_ms(300);
		OLED_Print(".\n");
		_delay_ms(500);
		
		OLED_Clear_Screen();
		
		OLED_Reset();
		_delay_ms(2000);
	}
}