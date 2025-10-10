#ifndef UI_H
#define UI_H

void draw_window(int X, int Y, unsigned char width_in_tiles, unsigned char height_in_tiles);
void draw_task_bar(void);
void joystick_indicator(char X, char Y, unsigned char hand);
void button_indicator(char X, char Y, unsigned char hand, unsigned char number);


#endif