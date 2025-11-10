#include "timer.h"
#include "sam.h"
#include "types.h"

volatile uint8_t Flag_Point_Lose = false;

void Timer_Init(void)
{
    // =========================================================================
    // TIMER 0 -> Counter Channel 0 === ADC Sampling 
    // =========================================================================

    // Enable Timer Counter 0 clock
    PMC->PMC_PCER0 |= (1 << ID_TC0);
    
    // Configure TC0 Channel 0 for waveform mode with pin output
    TC0->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK4  // MCK/128 = 656.25kHz
                               | TC_CMR_WAVE                 // Waveform mode
                               | TC_CMR_WAVSEL_UP_RC         // Count up with RC compare
                               | TC_CMR_ACPA_CLEAR           // Clear TIOA on RA compare
                               | TC_CMR_ACPC_SET;            // Set TIOA on RC compare
    
    // Set up for 1kHz output (50% duty cycle for visibility)
    TC0->TC_CHANNEL[0].TC_RC = 656;   // Period
    TC0->TC_CHANNEL[0].TC_RA = 328;   // 50% duty cycle
    
    // Start the timer
    TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;

    // =========================================================================
    // TIMER 0 -> Counter Channel 1 === Score delay 
    // =========================================================================
    
    // Enable Timer Counter 0 channel 1
    PMC->PMC_PCER0 |= (1 << ID_TC1);

    // Configure TC0 Channel 0 for waveform mode with pin output
    TC0->TC_CHANNEL[1].TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK5  // MCK/1024 = 84MHz/1024
                               | TC_CMR_WAVE                // Waveform mode
                               | TC_CMR_WAVSEL_UP_RC        // Count up with RC compare
                               | TC_CMR_ACPA_CLEAR          // Clear TIOA on RA compare
                               | TC_CMR_ACPC_SET;           // Set TIOA on RC compare
    
    // Set up for 1kHz output
    TC0->TC_CHANNEL[1].TC_RC = 82030;    // ~1 s period

    // Enable interrupt on RC Compare 
    TC0->TC_CHANNEL[1].TC_IER = TC_IER_CPCS;
    
    // Enable Timer 1 interrupt in NVIC
    NVIC_EnableIRQ(ID_TC1);

    // =========================================================================
    // TIMER 2 -> Counter Channel 0 === Optical encoder 
    // =========================================================================

    // Enable Timer Counter 0 clock
    PMC->PMC_PCER1 |= (1 << (ID_TC6 - 32));

    // Change block control register to enable quadrature mode
    TC2->TC_BMR = TC_BMR_QDEN 
                | TC_BMR_TC0XC0S_TCLK0      //Connect XCO to TIMER 2
                | TC_BMR_POSEN              // Enable position reading
                | TC_BMR_SPEEDEN            // Enable speed reading
                | TC_BMR_EDGPHA;            // Enable edge detection on PHA and PHB

    TC2->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_XC0;
    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

void Counter_Lose_Score_Start(void)
{
    Flag_Point_Lose = false;

    TC0->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

void Counter_Lose_Score_Stop(void)
{
    Flag_Point_Lose = false;
    TC0->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS;    
}

void Counter_Lose_Score_Restart(void)
{
    Flag_Point_Lose = false;
    
    // First stop, then start with reset
    TC0->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS;           
    TC0->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

// Timer 1 Interrupt Handler
void TC1_Handler(void)
{
    uint32_t status = TC0->TC_CHANNEL[1].TC_SR;  // Reading clears interrupt
    
    if (status & TC_SR_CPCS)
    {
        Flag_Point_Lose = true;
        // Auto-stop timer after it fires
        TC0->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS;
    }
}