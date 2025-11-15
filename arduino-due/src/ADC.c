/**
 * @file ADC.c
 * @brief ADC Init and Handler for scoring detection
 */

// =============================================================================
// INCLUDES
// =============================================================================

// Personal headers
#include "ADC.h"
#include "types.h"
#include "config.h"
#include "timer.h"
#include "drivers/can_controller.h"

// Libraries
#include <sam.h>

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

volatile uint16_t adc11_result = 0;
volatile uint8_t Flag_ADC;

// =============================================================================
// STATIC VARIABLES
// =============================================================================

// --- Averaging buffer ---
static uint16_t adc_buffer[ADC_AVG_COUNT];
static uint32_t adc_sum = 0;
static uint8_t adc_index = 0;
static bool adc_buffer_full = false;


// =============================================================================
// ADC FUNCTIONS
// =============================================================================

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
    uint32_t status = ADC->ADC_ISR;
    if (status & ADC_ISR_EOC11)
    {
        uint16_t new_sample = ADC->ADC_CDR[11] & 0x0FFF;  // Mask to 12 bits

        // --- Update rolling average buffer ---
        adc_sum -= adc_buffer[adc_index];   // Subtract old value
        adc_buffer[adc_index] = new_sample; // Store new one
        adc_sum += new_sample;              // Add new value

        adc_index++;
        if (adc_index >= ADC_AVG_COUNT)
        {
            adc_index = 0;
            adc_buffer_full = true;
        }

        // Compute average only when buffer is filled at least once
        if (adc_buffer_full)
            adc11_result = adc_sum / ADC_AVG_COUNT;
        else
            adc11_result = adc_sum / adc_index; // partial average during startup

        Flag_ADC = 1;
    }
}

// =============================================================================
// SCORING SYSTEM 
// =============================================================================

void handle_scoring_system(void)
{
    static uint8_t score = 0;
    static bool timer_started = false;
    static bool point_was_scored = false;
    static CANMessage mb0_buffer;

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

    if (Flag_Point_Lose && !point_was_scored)
    {
        score++;
        printf("POINT SCORED! Total: %d\n", score);
        point_was_scored = true;
        
        mb0_buffer.id = 8;
        mb0_buffer.data_length = 1;
        mb0_buffer.data[0] = score;

        if (can_send(&mb0_buffer, 0) == 0) {
            printf("Score sent: %d\n", score);
        }

        timer_started = false;
        Flag_Point_Lose = 0;
    }
}
