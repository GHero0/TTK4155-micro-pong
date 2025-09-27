#ifndef OLED_H_
#define OLED_H_

#include <avr/io.h>

#define DISP_DC PB2 // To send Data or Command
#define DISP_RES PB1 // To reset the display device

void OLED_DISP_DC_Data(void); // Set D/C = DATA
void OLED_DISP_DC_Command(void); // Set D/C = COMMAND

void OLED_Init(void);
void OLED_Set_Window(unsigned char line, unsigned char col_start, unsigned char col_end);
void OLED_FillLine(unsigned char line, unsigned char val);
void OLED_Clear_Screen(void);
void OLED_FillScreen(unsigned char val);
void OLED_Home(void);
void OLED_Go_To_Pos(unsigned char line_index, unsigned char column_index);
void OLED_PutChar(unsigned char c);
void OLED_Print(unsigned char *str);


#endif /* OLED_H_ */