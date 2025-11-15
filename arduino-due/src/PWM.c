#include "PWM.h"
#include "config.h"
#include "sam.h"
#include "can_messages.h"

void PWM_Init(void)
{   
    // for Servo        
    PMC->PMC_PCER1 |= (1 << (ID_PWM - 32));
    PIOB->PIO_PDR |= PIO_PDR_P13;
    PIOB->PIO_ABSR |= PIO_ABSR_P13;
    PIOB->PIO_PUDR |= PIO_PUDR_P13;
    
    PWM->PWM_CLK = PWM_CLK_PREA(0) | PWM_CLK_DIVA(42); // CLKA = 2 MHz
    PWM->PWM_CH_NUM[1].PWM_CMR = PWM_CMR_CPRE_CLKA;
    PWM->PWM_CH_NUM[1].PWM_CPRD = 40000;
    PWM->PWM_CH_NUM[1].PWM_CDTY = 38000;
    PWM->PWM_ENA |= (1 << 1);
    
    // for DC Motor
    PIOB->PIO_PDR |= PIO_PDR_P12;
    PIOB->PIO_ABSR |= PIO_ABSR_P12;
    PIOB->PIO_PUDR |= PIO_PUDR_P12;
    
    PWM->PWM_CH_NUM[0].PWM_CMR = PWM_CMR_CPRE_CLKA;
    PWM->PWM_CH_NUM[0].PWM_CPRD = 100;
    PWM->PWM_CH_NUM[0].PWM_CDTY = 100;
    PWM->PWM_ENA |= (1 << 0);
}

void PWM_Update(uint8_t value)
{
    int16_t adjusted = value + SERVO_OFFSET;

    // Apply deadzone and center correctly
    if (adjusted > SERVO_CENTER - SERVO_DEADZONE && adjusted < SERVO_CENTER + SERVO_DEADZONE)
        adjusted = SERVO_CENTER;

    PWM->PWM_CH_NUM[1].PWM_CDTYUPD = 38000 - (adjusted * 20.0);
}

void PWM_Motor(int fraction)
{
    fraction = clamp_int(fraction, 0, 100);
    PWM->PWM_CH_NUM[0].PWM_CDTYUPD = (100 - fraction);
}