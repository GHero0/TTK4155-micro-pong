#ifndef ADC_H
#define ADC_H

#include "types.h"

void ADC_Init(void);
void handle_scoring_system(void);

extern volatile uint16_t adc11_result;
extern volatile uint8_t Flag_ADC;

#endif