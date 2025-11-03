/**
 * @file IO_board.c
 * @brief IO_board Driver and global variables declaration
 */
#include "drivers/IO_board.h"
#include "types.h"
#include "config.h"
#include "drivers/SPI.h"

#include <util/delay.h>

Buttons buttons = {{0},{0},{0}};
TouchPad touch_pad = {0,0,0};
unsigned char slider = 0;  
LedState leds[6] = {0};

static void IO_board_read_buttons(void)
{
    SPI_Select_Slave(1);
    SPI_Write_byte(0x04); // COMMAND Select Buttons Address
    _delay_us(40);
    buttons.right = SPI_Read_byte();
    _delay_us(2);
    buttons.left = SPI_Read_byte();
    _delay_us(2);
    buttons.nav = SPI_Read_byte();
    SPI_Select_Slave(0);
}

static void IO_board_read_slider(void)
{
    SPI_Select_Slave(1);
    SPI_Write_byte(0x02); // COMMAND Select SLIDER Address
    _delay_us(40);
    slider = SPI_Read_byte();
    SPI_Select_Slave(0);
}

static void IO_board_read_touchpad(void)
{
    SPI_Select_Slave(1);
    SPI_Write_byte(0x01); // COMMAND Select TouchPAD Address
    _delay_us(40);
    touch_pad.x = SPI_Read_byte();
    _delay_us(2);
    touch_pad.y = SPI_Read_byte();
    _delay_us(2);
    touch_pad.size = SPI_Read_byte();
    SPI_Select_Slave(0);
}

static void IO_board_write_leds(void)
{
    for (unsigned char i = 0; i < LED_COUNT; i++)
    {
        SPI_Select_Slave(1);
        SPI_Write_byte(0x05); // COMMAND Select LEDS ON/OFF Address
        _delay_us(40);
        SPI_Write_byte(i);
        _delay_us(2);
        SPI_Write_byte(leds[i].on);
        _delay_us(2);
        SPI_Select_Slave(0);
    }
}

static void IO_board_write_pwm(void)
{
    for (unsigned char i = 0; i < LED_COUNT; i++)
    {
        SPI_Select_Slave(1);
        SPI_Write_byte(0x06); // COMMAND Select LEDS PWM Address
        _delay_us(40);
        SPI_Write_byte(i);
        _delay_us(2);
        SPI_Write_byte(leds[i].pwm);
        _delay_us(2);
        SPI_Select_Slave(0);
    }
}

void IO_board_update(void)
{
    IO_board_read_buttons();
    IO_board_read_slider();
    IO_board_read_touchpad();
    IO_board_write_leds();
    IO_board_write_pwm();
}