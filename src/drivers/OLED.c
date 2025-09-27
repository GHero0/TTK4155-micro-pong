#include "drivers/SPI.h"
#include "drivers/OLED.h"
#include "fonts.h"
#include "global.h"

#include <stdio.h>
#include <util/delay.h>

unsigned char pos_line = 0, pos_column = 0; // Cursor position

void OLED_DISP_DC_Data(void) { PORTB |= (1 << DISP_DC); }   // DATA
void OLED_DISP_DC_Command(void) { PORTB &= ~(1 << DISP_DC); } // CMD

void OLED_Init(void)
{
    // Set outputs
    PORTB |= (1 << SS2) | (1 << PB1); // CS high, RES high
    PORTB &= ~(1 << DISP_DC);         // D/C = 0
    DDRB |= (1 << DISP_DC) | (1 << PB1) | (1 << SS2);

    // Hardware reset
    PORTB &= ~(1 << PB1);
    _delay_ms(1);
    PORTB |= (1 << PB1);
    _delay_ms(1);

    SPI_Init();
    SPI_Select_Slave(2);
    _delay_us(40);

    // OLED init commands
    OLED_DISP_DC_Command();
    SPI_Write_byte(0xAE); // Display OFF

    SPI_Write_byte(0xD5); SPI_Write_byte(0x00); // Clock divide ratio
    SPI_Write_byte(0xA8); SPI_Write_byte(0x3F); // Multiplex 1/64
    SPI_Write_byte(0xD3); SPI_Write_byte(0x00); // Display offset
    SPI_Write_byte(0x40); // Start line
    SPI_Write_byte(0xA1); // Segment remap
    SPI_Write_byte(0xC8); // COM scan direction
    SPI_Write_byte(0xDA); SPI_Write_byte(0x12); // COM pins config
    SPI_Write_byte(0x81); SPI_Write_byte(0x7F); // Contrast
    SPI_Write_byte(0xA4); // Resume RAM display
    SPI_Write_byte(0xA6); // Normal display
    SPI_Write_byte(0xD9); SPI_Write_byte(0x11); // Pre-charge
    SPI_Write_byte(0xDB); SPI_Write_byte(0x20); // VCOMH
    SPI_Write_byte(0xAF); // Display ON
    SPI_Write_byte(0x20); SPI_Write_byte(0x00); // Horizontal addressing
    SPI_Write_byte(0x21); SPI_Write_byte(0x00); SPI_Write_byte(0x7F); // Column 0..127
    SPI_Write_byte(0x22); SPI_Write_byte(0x00); SPI_Write_byte(0x07); // Page 0..7
    SPI_Write_byte(0x2E); // Deactivate scroll

    SPI_Select_Slave(0);

    OLED_Clear_Screen();
    OLED_Home();
}

void OLED_Set_Window(unsigned char line, unsigned char col_start, unsigned char col_end)
{
    OLED_DISP_DC_Command();
    SPI_Select_Slave(2);

    SPI_Write_byte(0x21); // Column address
    SPI_Write_byte(col_start);
    SPI_Write_byte(col_end);

    SPI_Write_byte(0x22); // Page address
    SPI_Write_byte(line);
    SPI_Write_byte(line);
}

void OLED_FillLine(unsigned char line, unsigned char val)
{
    OLED_Set_Window(line, 0, 127);
    OLED_DISP_DC_Data();
    SPI_Select_Slave(2);
    for (int i = 0; i < 128; i++) SPI_Write_byte(val);
    SPI_Select_Slave(0);
}

void OLED_Clear_Screen(void)
{
    for (int l = 0; l < 8; l++) OLED_FillLine(l, 0x00);
}

void OLED_FillScreen(unsigned char val)
{
    for (int l = 0; l < 8; l++) OLED_FillLine(l, val);
}

void OLED_Home(void)
{
    pos_line = 0;
    pos_column = 0;
    OLED_Set_Window(pos_line, pos_column, 127);
}

void OLED_Go_To_Pos(unsigned char line_index, unsigned char column_index)
{
    pos_line = (line_index > 7) ? 7 : line_index;
    pos_column = (column_index > 127) ? 127 : column_index;
    OLED_Set_Window(pos_line, pos_column, 127);
}

void OLED_PutChar(unsigned char c)
{
    if (c < 32 || c > 126) c = '?';

    if (pos_column > 122) { // Wrap line
        pos_line++; pos_column = 0;
        if (pos_line > 7) return;
    }

    unsigned char b0 = pgm_read_byte(&(font5[c - 32][0]));
    unsigned char b1 = pgm_read_byte(&(font5[c - 32][1]));
    unsigned char b2 = pgm_read_byte(&(font5[c - 32][2]));
    unsigned char b3 = pgm_read_byte(&(font5[c - 32][3]));
    unsigned char b4 = pgm_read_byte(&(font5[c - 32][4]));

    OLED_Set_Window(pos_line, pos_column, 127);
    OLED_DISP_DC_Data();
    SPI_Select_Slave(2);
    SPI_Write_byte(b0);
    SPI_Write_byte(b1);
    SPI_Write_byte(b2);
    SPI_Write_byte(b3);
    SPI_Write_byte(b4);
    SPI_Write_byte(0x00); // spacing
    SPI_Select_Slave(0);

    pos_column += 6;
}

void OLED_Print(unsigned char *str)
{
    while (*str) OLED_PutChar(*str++);
}