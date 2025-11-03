#ifndef OLED_H
#define OLED_H

#include <avr/io.h>

#define DISP_DC PB2 // To send Data or Command
#define DISP_RES PB1 // To reset the display device

void OLED_Init(void);
void FrameBuffer_Init(void);
void FrameBuffer_Clear(void);
void FrameBuffer_Push(void);
void FrameBuffer_Swap(void);

// OLED frame buffer
extern unsigned char * volatile framebuffer;
extern unsigned char * volatile doublebuffer;
// Pointer aliases for convenience
extern unsigned char * volatile current_buffer;// draw here
extern unsigned char * volatile screen_buffer;  // send to OLED

#endif 