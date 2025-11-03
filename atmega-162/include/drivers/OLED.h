#ifndef OLED_H_
#define OLED_H_

#include <avr/io.h>

#define DISP_DC PB2 // To send Data or Command
#define DISP_RES PB1 // To reset the display device

void OLED_Init(void);
void FrameBufferInit(void);
void FrameBufferClear(void);
void FrameBufferPush(void);
void FrameBufferSwap(void);

// OLED frame buffer
extern unsigned char * volatile framebuffer;
extern unsigned char * volatile doublebuffer;
// Pointer aliases for convenience
extern unsigned char * volatile current_buffer;// draw here
extern unsigned char * volatile screen_buffer;  // send to OLED

#endif 