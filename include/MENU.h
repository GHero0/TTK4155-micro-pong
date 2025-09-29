#ifndef MENU_H_
#define MENU_H_

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>

typedef enum 
{
	MENU_MAIN = 0,
	MENU_SM1,
	MENU_SM2
} MenuLevel;

static MenuLevel level = MENU_MAIN;
static uint8_t sel = 0;

static JOY_DIR prev_dir = NEUTRAL; // Joystick's previous direction
static uint8_t prev_nav_pressed = 0;  // IO_board "nav" button state

// Small functions to help
static inline uint8_t wrap_down(uint8_t idx, uint8_t count) 
{
	return (idx == 0) ? (uint8_t)(count - 1) : (uint8_t)(idx - 1);
}
static inline uint8_t wrap_up(uint8_t idx, uint8_t count) 
{
	return ((uint8_t)(idx + 1) >= count) ? 0 : (uint8_t)(idx + 1);
}

void MENU_Init(void);
void MENU_Update(void);

// test_functions
void test_MENU(void);

#endif /* MENU_H_ */