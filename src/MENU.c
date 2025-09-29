#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>

#include "MENU.h"
#include "drivers/OLED.h"
#include "drivers/IO_board.h"
#include "constants.h"
#include "joystick.h"
#include "global.h"

// Joystick button state on PB0
static inline void JoyBtn_Init(void) 
{
	// PB0 as input + pull-up
	DDRB &= ~(1 << PB0);
	PORTB |= (1 << PB0);
}
static inline bool JoyBtn_IsPressed(void) 
{
	// Active low
	return ((PINB & (1 << PB0)) == 0);
}
static inline bool JoyBtn_WasPressed(void) 
{
	static bool prev = false;
	bool now = JoyBtn_IsPressed();
	bool edge = (!prev && now); // rising (pressed) edge
	prev = now;
	return edge;
}

// Menu (editable if something is modified)
#define MAIN_COUNT 3
#define SM1_COUNT 4
#define SM2_COUNT 3

static const char* main_items[MAIN_COUNT] = 
{ 
	"Launch Game",
	"Sub-Menu 1",
	"Sub-Menu 2"
};

static const char* sm1_items[SM1_COUNT] = 
{ 
	"SM1 - Option 1",
	"SM1 - Option 2",
	"SM1 - Option 3",
	"Back" 
};

static const char* sm2_items[SM2_COUNT] = 
{ 
	"SM2 - Option 1",
	"SM2 - Option 2",
	"Back" 
};

// Menu drawing
static void MENU_Draw(void)
{
	const char** items = 0;
	uint8_t count = 0;

	// MENU_Main
	if (level == MENU_MAIN) 
	{
		items = main_items; count = MAIN_COUNT;
	} 
	// MENU_SM1
	else if (level == MENU_SM1) 
	{
		items = sm1_items;  count = SM1_COUNT;
	} 
	// MENU_SM2
	else 
	{ 
		items = sm2_items;  count = SM2_COUNT;
	}

	// Clear screen and print lines
	OLED_Clear_Screen();
	OLED_Home();

	for (uint8_t i = 0; i < count; ++i) 
	{
		if (i == sel) 
		{
			OLED_Print(">");
		} 
		else 
		{
			OLED_Print(" ");
		}
		OLED_Print((char*)items[i]);
		OLED_PutChar('\n');
	}
}

// Enter/Validate (with the navigation button click)
static void MENU_Select(void)
{
	if (level == MENU_MAIN) 
	{
		// Launch game (placeholder) /* Possible to modify */
		if (sel == 0) {
			OLED_Clear_Screen();
			OLED_Home();
			OLED_Print("Launching game...\n");
			_delay_ms(700);
			MENU_Draw();
		} 
		// Sub-Menu 1 selected
		else if (sel == 1) 
		{
			level = MENU_SM1; 
			sel = 0;
			MENU_Draw();
		}
		// Sub-Menu 2 selected
		else if (sel == 2) 
		{
			level = MENU_SM2; 
			sel = 0;
			MENU_Draw();
		}
		return;
	}

	// Sub-Menus: last item ("Back")
	if (level == MENU_SM1) 
	{
		if (sel == (SM1_COUNT - 1)) 
		{
			level = MENU_MAIN; 
			sel = 0;
			MENU_Draw();
		} 
		// Placeholder: to implement for SM1
		else 
		{
			OLED_Clear_Screen();
			OLED_Home();
			OLED_Print("SM1 action...\n");
			_delay_ms(500);
			MENU_Draw();
		}
		return;
	}

	if (level == MENU_SM2) 
	{
		if (sel == (SM2_COUNT - 1)) 
		{
			level = MENU_MAIN; 
			sel = 0;
			MENU_Draw();
		}
		// Placeholder: to implement for SM2 
		else 
		{
			OLED_Clear_Screen();
			OLED_Home();
			OLED_Print("SM2 action...\n");
			_delay_ms(500);
			MENU_Draw();
		}
		return;
	}
}

void MENU_Init(void)
{
	// Init PB0 button
	JoyBtn_Init();
	
	level = MENU_MAIN;
	sel = 0;
	prev_dir = NEUTRAL;
	prev_nav_pressed = 0;

	MENU_Draw();
}

void MENU_Update(void)
{
	// Refresh IO_board (states/values : buttons, slider, touchpad)
	IO_board_update();

	JOY_DIR dir = joystick_dir;
	uint8_t nav_pressed = buttons.NB ? 1 : 0;
	bool need_redraw = false;

	// Move selection on UP/DOWN edge (joystick)
	if (dir == UP && prev_dir != UP) 
	{
		uint8_t count = (level == MENU_MAIN) ? MAIN_COUNT : (level == MENU_SM1)  ? SM1_COUNT  : SM2_COUNT;
		sel = wrap_down(sel, count);
		need_redraw = true;
	} 
	else if (dir == DOWN && prev_dir != DOWN) 
	{
		uint8_t count = (level == MENU_MAIN) ? MAIN_COUNT : (level == MENU_SM1)  ? SM1_COUNT  : SM2_COUNT;
		sel = wrap_up(sel, count);
		need_redraw = true;
	}

	// Validate/Enter on rising edge of IO_board nav button
	if (nav_pressed && !prev_nav_pressed) 
	{
		MENU_Select();
		need_redraw = false; // already redrawn inside
	}

	// Joystick button (PB0) -> immediate return to MAIN on press
	if (JoyBtn_WasPressed()) 
	{
		if (level != MENU_MAIN) 
		{
			level = MENU_MAIN;
			sel = 0;
			need_redraw = true;
		}
	}

	if (need_redraw) 
	{
		MENU_Draw();
	}

	prev_dir = dir;
	prev_nav_pressed = nav_pressed;
}