#include "drivers/SPI.h"
#include "drivers/OLED.h"
#include "MENU.h"
#include "constants.h"
#include "drivers/IO_board.h"
#include "joystick.h"
#include "global.h"
#include <stdio.h>

#include "util/delay.h"

void test_MENU(void)
{
	MENU_Update();
}