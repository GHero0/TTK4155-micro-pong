#include "drivers/SPI.h"
#include "drivers/OLED.h"
#include "global.h"

#include "util/delay.h"

void test_OLED()
{
	// Algorithm : TEST
	OLED_Home();
	OLED_Go_To_Pos(0, 20);
	OLED_Print("Hello guys!\n");
	_delay_ms(400);
	OLED_Print("We are\n");
	_delay_ms(400);
	OLED_Go_To_Pos(2, 20 + 30);
	OLED_Print("here\n");
	_delay_ms(400);
	OLED_Go_To_Pos(3, 20 + 60);
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

	_delay_ms(2000);
}