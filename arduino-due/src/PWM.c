#include "PWM.h"
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


// Takes value as an unsigned char between [0,100]
void PWM_Update(unsigned char value){
    // The period is fixed: 20ms
    // The only thing that does change it's the pulse width: 0.9ms -  2.1ms 
    PWM->PWM_CH_NUM[1].PWM_CDTYUPD = 38000-value*2*10;

}

// fraction is value between [0,100]
void PWM_Motor(int fraction){
	
	PWM->PWM_CH_NUM[0].PWM_CDTYUPD = (100-fraction)*1;
	//PWM->PWM_CH_NUM[1].PWM_CDTYUPD = (100-fraction)*400;
}
