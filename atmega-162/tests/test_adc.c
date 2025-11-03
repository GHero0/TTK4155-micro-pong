#include "tests/test_adc.h"
#include "SRAM.h"
#include "inter.h"
#include <stdio.h>


void test_adc(void)
{
    if (Flag_ADC_ready == 1)
    {
        Flag_ADC_ready = 0;
    }
    *adc = 0x01;
    printf("%3d | %3d | %3d | %3d\n", adc_ch0, adc_ch1, adc_ch2, adc_ch3);
}
