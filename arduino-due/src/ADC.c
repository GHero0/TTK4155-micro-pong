#include "ADC.h"
#include "types.h"
#include "config.h"
#include "timer.h"
#include "drivers/can_controller.h"

#include <sam.h>

volatile uint16_t adc11_result = 0;
volatile uint8_t Flag_ADC;

void ADC_Init(void)
{
    // Power up the ADC Clock 
    PMC->PMC_PCER1 |= (1 << (ID_ADC - 32));   

    // Disable PIO giving control to the ADC
    PIOB->PIO_PDR |= PIO_PDR_P18;

    // Optional: Disable pull-up
    PIOB->PIO_PUDR |= PIO_PUDR_P18;

    // ADC RESET
    ADC->ADC_CR = ADC_CR_SWRST;

    for(volatile int i = 0; i < 100; i++);
    // Conversion mode set to hardware trigger and hardware link to timer/counter 0
    ADC->ADC_MR = ADC_MR_TRGEN_EN           // Hardware trigger enabled
    | ADC_MR_TRGSEL_ADC_TRIG1   // Timer trigger
    | ADC_MR_PRESCAL(4)         // Prescaler: ADC clock = MCK/((4+1)*2) = 8.4MHz
    | ADC_MR_STARTUP_SUT64      // Startup time: 64 periods
    | ADC_MR_SETTLING_AST3      // Settling time: 3 periods
    | ADC_MR_TRACKTIM(15)       // Tracking time: 15 periods
    | ADC_MR_TRANSFER(2);       // Transfer period: 2 periods

    // ADC Channel 11 Enable correspond to PIN PB20/ADC[11]
    ADC->ADC_CHER = ADC_CHER_CH11;

    // Enable conversion
    ADC->ADC_IER = ADC_IER_EOC11;  // End of conversion interrupt for channel 11
    NVIC_EnableIRQ(ADC_IRQn);
}

// ADC Interrupt Handler
void ADC_Handler(void)
{
    // Read status to clear interrupt flag
    uint32_t status = ADC->ADC_ISR;
    // printf("Interrupt triggered\n");
    // Check if channel 11 conversion is complete
    if (status & ADC_ISR_EOC11)
    {
        // Read conversion result (12-bit value in bits 0-11)
        adc11_result = ADC->ADC_CDR[11] & 0x0FFF;  // Mask to 12 bits
        Flag_ADC = 1;
    }

}


// =============================================================================
// SCORING SYSTEM 
// =============================================================================

void handle_scoring_system(void)
{
    // Keep ONLY the static versions
    static bool send_pending = false;
    static uint8_t score = 0;
    static bool timer_started = false;
    static bool point_was_scored = false;
    static CANMessage mb0_buffer;

    // Handle ADC ball detection
    if (Flag_ADC)
    {
        if (adc11_result < BALL_DETECT_THRESHOLD)
        {
            if (!timer_started && !point_was_scored) {
                Counter_Lose_Score_Start();
                timer_started = true;
            }
        }
        else if (adc11_result > BALL_CLEAR_THRESHOLD)
        {
            if (timer_started) {
                Counter_Lose_Score_Stop();
                timer_started = false;
            }
            if (point_was_scored) {
                point_was_scored = false;
            }
        }
        Flag_ADC = 0;
    }

    // Handle point scoring
    if (Flag_Point_Lose && !point_was_scored)
    {
        score++;
        printf("POINT SCORED! Total: %d\n", score);
        point_was_scored = true;
        
        mb0_buffer.id = 1;
        mb0_buffer.data_length = 1;
        mb0_buffer.data[0] = score;
        send_pending = true;
        
        timer_started = false;
        Flag_Point_Lose = 0;
    }
    
    // Non-blocking send (single attempt per cycle)
    if (send_pending)
    {
        if (can_send(&mb0_buffer, 0) == 0) {
            printf("Score sent: %d\n", score);
            send_pending = false;
        }
    }
}