#include "PWM.h"
#include "config.h"

#include "sam.h"

void PWM_Init(void)
{	
	// for Servo		
    //  Power enable for PWM
    PMC->PMC_PCER1 |= (1 << (ID_PWM - 32));
    // Disable PIO giving control to the PWM
    PIOB->PIO_PDR |= PIO_PDR_P13;
    // PWM Select Peripheral B and pin 
    PIOB->PIO_ABSR |= PIO_ABSR_P13;
    // Optional: Disable pull-up
    PIOB->PIO_PUDR |= PIO_PUDR_P13;

    
    // Configuration of PWM ~[0.9ms, 2.1ms] and duty cycle variable
    PWM->PWM_CLK = PWM_CLK_PREA(0) | PWM_CLK_DIVA(42); // CLKA = 2 MHz
    PWM->PWM_CH_NUM[1].PWM_CMR = PWM_CMR_CPRE_CLKA;
    PWM->PWM_CH_NUM[1].PWM_CPRD = 40000;
    PWM->PWM_CH_NUM[1].PWM_CDTY = 37000;
    PWM->PWM_ENA = (1 << 1);

    PWM->PWM_CH_NUM[1].PWM_CDTY = 36000;
    PWM->PWM_ENA |= (1 << 1);
	
	
	
	//for DC Motor
	//  Power enable for PWM
	PMC->PMC_PCER1 |= (1 << (ID_PWM - 32));
	// Disable PIO giving control to the PWM	// PB12
	PIOB->PIO_PDR |= PIO_PDR_P12;
	// PWM Select Peripheral B and pin
	PIOB->PIO_ABSR |= PIO_ABSR_P12;
	// Optional: Disable pull-up
	PIOB->PIO_PUDR |= PIO_PUDR_P12;
	
	// Configuration of PWM ~[0.9ms, 2.1ms] and duty cycle variable
	PWM->PWM_CLK = PWM_CLK_PREA(0) | PWM_CLK_DIVA(42);	// CLKA = 2 MHz, shares the same clock with servo PWM
	PWM->PWM_CH_NUM[0].PWM_CMR = PWM_CMR_CPRE_CLKA;		// use clock A 
	PWM->PWM_CH_NUM[0].PWM_CPRD = 100;				// f_PWM = 2MHz/100 = 20kHz, period of 50us
	PWM->PWM_CH_NUM[0].PWM_CDTY = 100;				// Duty Cycle (low time) -> completly of
	PWM->PWM_ENA |= (1 << 0);
	
}


#define SERVO_CENTER 56
#define SERVO_DEADZONE 2
#define SERVO_OFFSET 6

void PWM_Update(uint8_t value)
{
    int16_t adjusted = value + SERVO_OFFSET;

    // Apply deadzone
    if (adjusted > SERVO_CENTER - SERVO_DEADZONE && adjusted < SERVO_CENTER + SERVO_DEADZONE)
        adjusted = SERVO_CENTER;

    PWM->PWM_CH_NUM[1].PWM_CDTYUPD = 38200 - (adjusted * 24);
}

static inline int clamp_int(int val, int min, int max)
{
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

uint8_t map_joystick_to_servo_fractional(int8_t joystick_int, uint8_t joystick_frac)
{
    // Clamp integer
    joystick_int = clamp_int(joystick_int, -100, 100);
    
    // Convert to high-resolution 16-bit fixed point: value * 256 + fraction
    int32_t value_16bit = ((int32_t)joystick_int << 8) + joystick_frac;

    // Map to 0..100 PWM with flipped direction:
    // -100 -> 100, 0 -> 50, +100 -> 0
    uint32_t temp = (uint32_t)(100*256 - (value_16bit + 100*256) / 2);

    // Convert back to 0..100 with rounding
    return (uint8_t)((temp + 128) / 256);
}

// fraction is value between [0,100]
void PWM_Motor(int fraction){
	
	PWM->PWM_CH_NUM[0].PWM_CDTYUPD = (100-fraction)*1;
}
