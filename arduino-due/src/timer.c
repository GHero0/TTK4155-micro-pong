#include "timer.h"
#include "sam.h"

void Timer_Init(void)
{
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
}

