#ifndef OLED_H_
#define OLED_H_

#include <avr/io.h>
#include <avr/pgmspace.h>

#define DISP_DC PB2 // To send Data or Command
#define DISP_RES PB1 // To reset the display device

#define OLED_COLUMN_OFFSET 0 // Add if needed, otherwise = 0

void OLED_DISP_DC_Data(void); // Set D/C = DATA
void OLED_DISP_DC_Command(void); // Set D/C = COMMAND

void OLED_Init(void);
void OLED_Reset(void);
void OLED_Home(void);
void OLED_Go_To_Line(int);
void OLED_Go_To_Column(int);
void OLED_Clear_Line(int);
void OLED_Go_To_Pos(int, int);
void OLED_PutChar(char);
void OLED_Print(char*);
void OLED_Clear_Screen(void);

void OLED_FillScreen(uint8_t, uint8_t);

// test_functions
void test_function_Clear_Screen_From_Noise(void);
void test_OLED(void);

#endif /* OLED_H_ */