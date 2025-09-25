#include "drivers/IO_board.h"
#include "drivers/SPI.h"
#include "global.h"

#include <util/delay.h>

void IO_board_read_buttons(void)
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

void IO_board_read_slider(void)
{
    SPI_Select_Slave(1);
    SPI_Write_byte(0x02); // COMMAND Select SLIDER Address
    _delay_us(40);
    slider = SPI_Read_byte();
    SPI_Select_Slave(0);
}

void IO_board_read_touchpad(void)
{
    SPI_Select_Slave(1);
    SPI_Write_byte(0x01); // COMMAND Select TouchPAD Address
    _delay_us(40);
    touch_x = SPI_Read_byte();
    _delay_us(2);
    touch_y = SPI_Read_byte();
    _delay_us(2);
    touch_size = SPI_Read_byte();
    SPI_Select_Slave(0);
}

void IO_board_write_leds(void)
{
    for (unsigned char i = 0; i < 6; i++)
    {
        SPI_Select_Slave(1);
        SPI_Write_byte(0x05); // COMMAND Select LEDS Address
        _delay_us(40);
        SPI_Write_byte(i);
        _delay_us(2);
        SPI_Write_byte((leds >> i) & 1);
        _delay_us(2);
        SPI_Select_Slave(0);
    }
}

void IO_board_update()
{
    IO_board_read_buttons();
    IO_board_read_slider();
    IO_board_read_touchpad();
    IO_board_write_leds();
}