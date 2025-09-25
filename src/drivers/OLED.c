#include "drivers/SPI.h"
#include "drivers/OLED.h"
#include "global.h"
#include <stdio.h>

#include <util/delay.h>

static uint8_t pos_line = 0, pos_column = 0; // Position of the cursor (with lines : 0-7 and columns : 0-127)

/* 
5x7 ASCII font (32..126) stored in PROGMEM.
Each char = 5 columns, LSB at top, plus 1 blank column added by code.
*/
static const uint8_t font5x7[] PROGMEM = {
  /* 0x20 ' ' */ 0x00,0x00,0x00,0x00,0x00,
  /* 0x21 '!' */ 0x00,0x00,0x5F,0x00,0x00,
  /* 0x22 '"'*/  0x00,0x07,0x00,0x07,0x00,
  /* 0x23 '#' */ 0x14,0x7F,0x14,0x7F,0x14,
  /* 0x24 '$' */ 0x24,0x2A,0x7F,0x2A,0x12,
  /* 0x25 '%' */ 0x23,0x13,0x08,0x64,0x62,
  /* 0x26 '&' */ 0x36,0x49,0x55,0x22,0x50,
  /* 0x27 '''*/  0x00,0x05,0x03,0x00,0x00,
  /* 0x28 '(' */ 0x00,0x1C,0x22,0x41,0x00,
  /* 0x29 ')' */ 0x00,0x41,0x22,0x1C,0x00,
  /* 0x2A '*' */ 0x14,0x08,0x3E,0x08,0x14,
  /* 0x2B '+' */ 0x08,0x08,0x3E,0x08,0x08,
  /* 0x2C ',' */ 0x00,0x50,0x30,0x00,0x00,
  /* 0x2D '-' */ 0x08,0x08,0x08,0x08,0x08,
  /* 0x2E '.' */ 0x00,0x60,0x60,0x00,0x00,
  /* 0x2F '/' */ 0x20,0x10,0x08,0x04,0x02,
  /* 0x30 '0' */ 0x3E,0x51,0x49,0x45,0x3E,
  /* 0x31 '1' */ 0x00,0x42,0x7F,0x40,0x00,
  /* 0x32 '2' */ 0x42,0x61,0x51,0x49,0x46,
  /* 0x33 '3' */ 0x21,0x41,0x45,0x4B,0x31,
  /* 0x34 '4' */ 0x18,0x14,0x12,0x7F,0x10,
  /* 0x35 '5' */ 0x27,0x45,0x45,0x45,0x39,
  /* 0x36 '6' */ 0x3C,0x4A,0x49,0x49,0x30,
  /* 0x37 '7' */ 0x01,0x71,0x09,0x05,0x03,
  /* 0x38 '8' */ 0x36,0x49,0x49,0x49,0x36,
  /* 0x39 '9' */ 0x06,0x49,0x49,0x29,0x1E,
  /* 0x3A ':' */ 0x00,0x36,0x36,0x00,0x00,
  /* 0x3B ';' */ 0x00,0x56,0x36,0x00,0x00,
  /* 0x3C '<' */ 0x08,0x14,0x22,0x41,0x00,
  /* 0x3D '=' */ 0x14,0x14,0x14,0x14,0x14,
  /* 0x3E '>' */ 0x00,0x41,0x22,0x14,0x08,
  /* 0x3F '?' */ 0x02,0x01,0x51,0x09,0x06,
  /* 0x40 '@' */ 0x32,0x49,0x79,0x41,0x3E,
  /* 0x41 'A' */ 0x7E,0x11,0x11,0x11,0x7E,
  /* 0x42 'B' */ 0x7F,0x49,0x49,0x49,0x36,
  /* 0x43 'C' */ 0x3E,0x41,0x41,0x41,0x22,
  /* 0x44 'D' */ 0x7F,0x41,0x41,0x22,0x1C,
  /* 0x45 'E' */ 0x7F,0x49,0x49,0x49,0x41,
  /* 0x46 'F' */ 0x7F,0x09,0x09,0x09,0x01,
  /* 0x47 'G' */ 0x3E,0x41,0x49,0x49,0x7A,
  /* 0x48 'H' */ 0x7F,0x08,0x08,0x08,0x7F,
  /* 0x49 'I' */ 0x00,0x41,0x7F,0x41,0x00,
  /* 0x4A 'J' */ 0x20,0x40,0x41,0x3F,0x01,
  /* 0x4B 'K' */ 0x7F,0x08,0x14,0x22,0x41,
  /* 0x4C 'L' */ 0x7F,0x40,0x40,0x40,0x40,
  /* 0x4D 'M' */ 0x7F,0x02,0x0C,0x02,0x7F,
  /* 0x4E 'N' */ 0x7F,0x04,0x08,0x10,0x7F,
  /* 0x4F 'O' */ 0x3E,0x41,0x41,0x41,0x3E,
  /* 0x50 'P' */ 0x7F,0x09,0x09,0x09,0x06,
  /* 0x51 'Q' */ 0x3E,0x41,0x51,0x21,0x5E,
  /* 0x52 'R' */ 0x7F,0x09,0x19,0x29,0x46,
  /* 0x53 'S' */ 0x46,0x49,0x49,0x49,0x31,
  /* 0x54 'T' */ 0x01,0x01,0x7F,0x01,0x01,
  /* 0x55 'U' */ 0x3F,0x40,0x40,0x40,0x3F,
  /* 0x56 'V' */ 0x1F,0x20,0x40,0x20,0x1F,
  /* 0x57 'W' */ 0x3F,0x40,0x38,0x40,0x3F,
  /* 0x58 'X' */ 0x63,0x14,0x08,0x14,0x63,
  /* 0x59 'Y' */ 0x07,0x08,0x70,0x08,0x07,
  /* 0x5A 'Z' */ 0x61,0x51,0x49,0x45,0x43,
  /* 0x5B '[' */ 0x00,0x7F,0x41,0x41,0x00,
  /* 0x5C '\' */ 0x02,0x04,0x08,0x10,0x20,
  /* 0x5D ']' */ 0x00,0x41,0x41,0x7F,0x00,
  /* 0x5E '^' */ 0x04,0x02,0x01,0x02,0x04,
  /* 0x5F '_' */ 0x40,0x40,0x40,0x40,0x40,
  /* 0x60 '`' */ 0x00,0x01,0x02,0x04,0x00,
  /* 0x61 'a' */ 0x20,0x54,0x54,0x54,0x78,
  /* 0x62 'b' */ 0x7F,0x48,0x44,0x44,0x38,
  /* 0x63 'c' */ 0x38,0x44,0x44,0x44,0x20,
  /* 0x64 'd' */ 0x38,0x44,0x44,0x48,0x7F,
  /* 0x65 'e' */ 0x38,0x54,0x54,0x54,0x18,
  /* 0x66 'f' */ 0x08,0x7E,0x09,0x01,0x02,
  /* 0x67 'g' */ 0x0C,0x52,0x52,0x52,0x3E,
  /* 0x68 'h' */ 0x7F,0x08,0x04,0x04,0x78,
  /* 0x69 'i' */ 0x00,0x44,0x7D,0x40,0x00,
  /* 0x6A 'j' */ 0x20,0x40,0x44,0x3D,0x00,
  /* 0x6B 'k' */ 0x7F,0x10,0x28,0x44,0x00,
  /* 0x6C 'l' */ 0x00,0x41,0x7F,0x40,0x00,
  /* 0x6D 'm' */ 0x7C,0x04,0x18,0x04,0x78,
  /* 0x6E 'n' */ 0x7C,0x08,0x04,0x04,0x78,
  /* 0x6F 'o' */ 0x38,0x44,0x44,0x44,0x38,
  /* 0x70 'p' */ 0x7C,0x14,0x14,0x14,0x08,
  /* 0x71 'q' */ 0x08,0x14,0x14,0x14,0x7C,
  /* 0x72 'r' */ 0x7C,0x08,0x04,0x04,0x08,
  /* 0x73 's' */ 0x48,0x54,0x54,0x54,0x20,
  /* 0x74 't' */ 0x04,0x3F,0x44,0x40,0x20,
  /* 0x75 'u' */ 0x3C,0x40,0x40,0x20,0x7C,
  /* 0x76 'v' */ 0x1C,0x20,0x40,0x20,0x1C,
  /* 0x77 'w' */ 0x3C,0x40,0x30,0x40,0x3C,
  /* 0x78 'x' */ 0x44,0x28,0x10,0x28,0x44,
  /* 0x79 'y' */ 0x0C,0x50,0x50,0x50,0x3C,
  /* 0x7A 'z' */ 0x44,0x64,0x54,0x4C,0x44,
  /* 0x7B '{' */ 0x00,0x08,0x36,0x41,0x00,
  /* 0x7C '|' */ 0x00,0x00,0x7F,0x00,0x00,
  /* 0x7D '}' */ 0x00,0x41,0x36,0x08,0x00,
  /* 0x7E '~' */ 0x08,0x04,0x08,0x10,0x08
};

void OLED_DISP_DC_Data(void){ PORTB |=  (1 << DISP_DC); } // DATA
void OLED_DISP_DC_Command(void) { PORTB &= ~(1 << DISP_DC); } // CMD

void OLED_Init(void)
{
	// Set the level of the outputs
	PORTB |=  (1 << SS2);     // CS HIGH (idle)
	PORTB &= ~(1 << DISP_DC); // D/C = 0 (command only)
	PORTB |=  (1 << PB1);     // RES HIGH (idle)
	
	DDRB |= (1 << DISP_DC) | (1 << PB1) | (1 << SS2);  // Set in output : D/C, RES, CS/SS2/DISP_CS
	
	// Default setting : will only take commands
	OLED_DISP_DC_Command();
	// DISPLAY_RES trigger/pulse
	PORTB &= ~(1 << PB1); // RES LOW
	_delay_ms(1);
	PORTB |= (1 << PB1); // RES HIGH
	_delay_ms(1);
	
	SPI_Init();
	
	// Choose the SS2 (2nd Slave)
	SPI_Select_Slave(2);
	_delay_us(40);
	
	// Display will only takes command
	OLED_DISP_DC_Command();
	
	// Display Off
	SPI_Write_byte(0xAE);
	_delay_us(3);
	// Set display clock divide ratio / oscillator frequency
	SPI_Write_byte(0xD5);
	_delay_us(3);
	SPI_Write_byte(0x80);
	_delay_us(3);
	// Set multiplex ratio (1/64 duty)
	SPI_Write_byte(0xA8);
	_delay_us(3);
	SPI_Write_byte(0x3F);
	_delay_us(3);
	// Set display offset (0)
	SPI_Write_byte(0xD3);
	_delay_us(3);
	SPI_Write_byte(0x00);
	_delay_us(3);
	// Set display start line (0)
	SPI_Write_byte(0x40);
	_delay_us(3);
	// Set segment re-map (col addr 127 ? SEG0)
	SPI_Write_byte(0xA1);
	_delay_us(3);
	// Set COM scan direction (remap mode)
	SPI_Write_byte(0xC8);
	_delay_us(3);
	// Set COM pins hardware configuration
	SPI_Write_byte(0xDA);
	_delay_us(3);
	SPI_Write_byte(0x12);
	_delay_us(3);
	// Set contrast control (0x7F = reset value)
	SPI_Write_byte(0x81);
	_delay_us(3);
	SPI_Write_byte(0x7F);
	_delay_us(3);
	// Resume to RAM content display (output follows RAM)
	SPI_Write_byte(0xA4);
	_delay_us(3);
	// Set normal display (not inverted)
	SPI_Write_byte(0xA6);
	_delay_us(3);
	// Set pre-charge period
	SPI_Write_byte(0xD9);
	_delay_us(3);
	SPI_Write_byte(0x22);
	_delay_us(3);
	// Set VCOMH deselect level
	SPI_Write_byte(0xDB);
	_delay_us(3);
	SPI_Write_byte(0x20);
	_delay_us(3);
	/* Useful if charge pump (Enable) implemented
	SPI_Write_byte(0x8D);
	_delay_us(3);
	SPI_Write_byte(0x14);
	_delay_us(3); 
	*/
	// Display ON
	SPI_Write_byte(0xAF);
	
	// addressing + full window + stop scroll
	SPI_Write_byte(0x20); 
	SPI_Write_byte(0x00); // Memory Addressing Mode = Horizontal
	SPI_Write_byte(0x21); 
	SPI_Write_byte(0x00); 
	SPI_Write_byte(0x7F); // Columns 0..127
	SPI_Write_byte(0x22); 
	SPI_Write_byte(0x00); 
	SPI_Write_byte(0x07); // Pages 0..7
	SPI_Write_byte(0x2E); // Deactivate scroll
	
	// To let the other slaves use the SPI bus
	SPI_Select_Slave(0);
}

void OLED_Reset(void)
{
	// Turn the display OFF before the reset
	OLED_DISP_DC_Command();
	SPI_Select_Slave(2);
	SPI_Write_byte(0xAE);
	SPI_Select_Slave(0);

	// Hardware reset pulse on RES
	PORTB &= ~(1 << PB1); 
	_delay_ms(1);
	PORTB |= (1 << PB1); 
	_delay_ms(1);

	// Reapply addressing + stop scroll + ON
	OLED_DISP_DC_Command();
	SPI_Select_Slave(2);
	SPI_Write_byte(0x20); 
	SPI_Write_byte(0x00); // Horizontal addressing
	SPI_Write_byte(0x21); 
	SPI_Write_byte(0x00 + OLED_COLUMN_OFFSET); 
	SPI_Write_byte(0x7F + OLED_COLUMN_OFFSET);
	SPI_Write_byte(0x22); 
	SPI_Write_byte(0x00);                   
	SPI_Write_byte(0x07);
	SPI_Write_byte(0x2E); // Idle scroll
	SPI_Write_byte(0xAF); // Display ON
	SPI_Select_Slave(0);

	OLED_Init();
	OLED_Home();
}

void OLED_Home(void)
{
	pos_line = 0;
	pos_column = 0;

	// Set window to line 0, column 0
	OLED_DISP_DC_Command();
	SPI_Select_Slave(2);
	SPI_Write_byte(0x21); 
	SPI_Write_byte(pos_column + OLED_COLUMN_OFFSET); 
	SPI_Write_byte(0x7F + OLED_COLUMN_OFFSET);
	SPI_Write_byte(0x22); 
	SPI_Write_byte(pos_line);                  
	SPI_Write_byte(pos_line);
	SPI_Select_Slave(0);
}

void OLED_Go_To_Line(int line_index)
{
	if (line_index < 0) 
	{
		line_index = 0; 
	}
	if (line_index > 7) 
	{
		line_index = 7;
	}
	pos_line = (uint8_t) line_index;

	OLED_DISP_DC_Command();
	SPI_Select_Slave(2);
	
	// keep current column (pos_column) to 127 on this line
	SPI_Write_byte(0x21); 
	SPI_Write_byte(pos_column + OLED_COLUMN_OFFSET); 
	SPI_Write_byte(0x7F + OLED_COLUMN_OFFSET);
	SPI_Write_byte(0x22); 
	SPI_Write_byte(pos_line);                 
	SPI_Write_byte(pos_line);
	SPI_Select_Slave(0);
}

void OLED_Go_To_Column(int column_index)
{
	if (column_index < 0) 
	{
		column_index = 0; 
	}
	if (column_index > 127) 
	{
		column_index = 127;
	}
	pos_column = (uint8_t) column_index;

	OLED_DISP_DC_Command();
	SPI_Select_Slave(2);
	// window on current line, from new pos_column to 127
	SPI_Write_byte(0x21); 
	SPI_Write_byte(pos_column + OLED_COLUMN_OFFSET); 
	SPI_Write_byte(0x7F + OLED_COLUMN_OFFSET);
	SPI_Write_byte(0x22); 
	SPI_Write_byte(pos_line);                  
	SPI_Write_byte(pos_line);
	SPI_Select_Slave(0);
}

void OLED_Clear_Line(int line_index)
{
	if (line_index < 0) line_index = 0;
	if (line_index > 7) line_index = 7;

	// Window: full width on this line
	OLED_DISP_DC_Command();
	SPI_Select_Slave(2);
	SPI_Write_byte(0x21);
	SPI_Write_byte(0x00 + OLED_COLUMN_OFFSET);
	SPI_Write_byte(0x7F + OLED_COLUMN_OFFSET);
	SPI_Write_byte(0x22);
	SPI_Write_byte((uint8_t)line_index);
	SPI_Write_byte((uint8_t)line_index);

	// Data: 128 zeros
	OLED_DISP_DC_Data();
	for (uint16_t i = 0; i < 128; ++i) SPI_Write_byte(0x00);
	SPI_Select_Slave(0);

	// If cursor is on this line, reset column and window for subsequent writes
	if (pos_line == (uint8_t)line_index) {
		pos_column = 0;
		OLED_DISP_DC_Command();
		SPI_Select_Slave(2);
		SPI_Write_byte(0x21);
		SPI_Write_byte(0x00 + OLED_COLUMN_OFFSET);
		SPI_Write_byte(0x7F + OLED_COLUMN_OFFSET);
		SPI_Write_byte(0x22);
		SPI_Write_byte(pos_line);
		SPI_Write_byte(pos_line);
		SPI_Select_Slave(0);
	}
}

void OLED_Go_To_Pos(int line_index, int column_index)
{
	if (line_index < 0) 
	{
		line_index = 0; 
	}
	if (line_index > 7)
	{ 
		line_index = 7;
	}
	if (column_index  < 0)
	{ 
		column_index  = 0; 
	}
	if (column_index  > 127) 
	{
		column_index  = 127;
	}
	pos_line = (uint8_t) line_index; pos_column = (uint8_t) column_index;

	OLED_DISP_DC_Command();
	SPI_Select_Slave(2);
	SPI_Write_byte(0x21); 
	SPI_Write_byte(pos_column + OLED_COLUMN_OFFSET); 
	SPI_Write_byte(0x7F + OLED_COLUMN_OFFSET);
	SPI_Write_byte(0x22); 
	SPI_Write_byte(pos_line);                 
	SPI_Write_byte(pos_line);
	SPI_Select_Slave(0);
}

/* Print one 5x7 character (+1 space column). Wrap to next line at end. */
void OLED_PutChar(char c)
{
	if (c == '\n') {
		if (pos_line < 7) 
		{ 
			pos_line++; 
			pos_column = 0;
		}
		// set new line window
		OLED_DISP_DC_Command();
		SPI_Select_Slave(2);
		SPI_Write_byte(0x21); 
		SPI_Write_byte(0x00 + OLED_COLUMN_OFFSET);  
		SPI_Write_byte(0x7F + OLED_COLUMN_OFFSET);
		SPI_Write_byte(0x22); 
		SPI_Write_byte(pos_line);                  
		SPI_Write_byte(pos_line);
		SPI_Select_Slave(0);
		return;
	}
	if (c < 32 || c > 126) 
	{
		c = '?';
	}

	// 6 columns per char (5 glyph/draw character + 1 spacing)
	if (pos_column > 122) { // not enough room for a char -> wrap
		if (pos_line < 7) 
		{ 
			pos_line++; 
			pos_column = 0; 
		}
		else 
		{
			return; // no space
		}
		OLED_DISP_DC_Command();
		SPI_Select_Slave(2);
		SPI_Write_byte(0x21); 
		SPI_Write_byte(0x00 + OLED_COLUMN_OFFSET);  
		SPI_Write_byte(0x7F + OLED_COLUMN_OFFSET);
		SPI_Write_byte(0x22); 
		SPI_Write_byte(pos_line);                  
		SPI_Write_byte(pos_line);
		SPI_Select_Slave(0);
	}

	uint16_t idx = (uint16_t)(c - 32) * 5; // 5 columns per glyph
	uint8_t b0 = pgm_read_byte(&font5x7[idx + 0]);
	uint8_t b1 = pgm_read_byte(&font5x7[idx + 1]);
	uint8_t b2 = pgm_read_byte(&font5x7[idx + 2]);
	uint8_t b3 = pgm_read_byte(&font5x7[idx + 3]);
	uint8_t b4 = pgm_read_byte(&font5x7[idx + 4]);

	// Ensure the window starts at current column (s_col..127 on current line)
	OLED_DISP_DC_Command();
	SPI_Select_Slave(2);
	SPI_Write_byte(0x21); 
	SPI_Write_byte(pos_column + OLED_COLUMN_OFFSET);     
	SPI_Write_byte(0x7F + OLED_COLUMN_OFFSET);
	SPI_Write_byte(0x22); 
	SPI_Write_byte(pos_line);                      
	SPI_Write_byte(pos_line);
	// Send 5 glyph columns + 1 spacing column
	OLED_DISP_DC_Data();
	SPI_Write_byte(b0); 
	SPI_Write_byte(b1); 
	SPI_Write_byte(b2); 
	SPI_Write_byte(b3); 
	SPI_Write_byte(b4);
	SPI_Write_byte(0x00); // space
	SPI_Select_Slave(0);

	pos_column += 6;
	if (pos_column > 127) pos_column = 127;
}

void OLED_Print(char* print_string)
{
	if (!print_string) 
	{
		return;
	}
	while (*print_string) 
	{
		OLED_PutChar(*print_string++);
	}
}

// Clear all 8 lines
void OLED_Clear_Screen(void)
{
	for (int l = 0; l < 8; ++l)
	{
		OLED_Clear_Line(l);
		_delay_ms(200);
	}
}

void OLED_FillScreen(uint8_t line, uint8_t val){
	// Only the first 8 lines/rows on the total width
	OLED_DISP_DC_Command();
	SPI_Select_Slave(2);
	SPI_Write_byte(0x21); 
	SPI_Write_byte(0x00); 
	SPI_Write_byte(0x7F); // columns 0..127
	SPI_Write_byte(0x22); 
	SPI_Write_byte(line); 
	SPI_Write_byte(line); // this line only
	OLED_DISP_DC_Data();
	for (int i = 0; i < 128; ++i) 
	{
		SPI_Write_byte(val); // 128 octets
	}
	SPI_Select_Slave(0);
}

/*
	AEh         ; Display Off
	D5h, 0x80   ; Set display clock divide ratio / oscillator frequency
	A8h, 0x3F   ; Set multiplex ratio (1/64 duty)
	D3h, 0x00   ; Set display offset (0)
	40h         ; Set display start line (0)
	A1h         ; Set segment re-map (col addr 127 ? SEG0)
	C8h         ; Set COM scan direction (remap mode)
	DAh, 0x12   ; Set COM pins hardware configuration
	81h, 0x7F   ; Set contrast control (0x7F = reset value)
	A4h         ; Resume to RAM content display (output follows RAM)
	A6h         ; Set normal display (not inverted)
	D9h, 0x22   ; Set pre-charge period
	DBh, 0x20   ; Set VCOMH deselect level
	8Dh, 0x14   ; Charge pump (Enable) (if implemented)
	AFh         ; Display ON
*/