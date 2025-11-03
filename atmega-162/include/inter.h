#ifndef INTER_H
#define INTER_H

void Inter_Init(void);

extern volatile char Flag_ADC_ready;
extern volatile char Flag_screen;
extern volatile char Flag_CAN;

extern unsigned char adc_ch0;
extern unsigned char adc_ch1;
extern unsigned char adc_ch2;
extern unsigned char adc_ch3;

#endif 