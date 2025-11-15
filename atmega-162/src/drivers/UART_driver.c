/**
 * @file UART_driver.c
 * @brief UART Driver Implementation functions along with stdio coupling
 */

// =============================================================================
// INCLUDES
// =============================================================================

// Personal headers
#include "drivers/UART_driver.h"

// Libraries
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

// =============================================================================
// CIRCULAR BUFFER FOR RECEIVED DATA
// =============================================================================

#define UART_RX_BUFFER_SIZE 64

static volatile char uart_rx_buffer[UART_RX_BUFFER_SIZE];
static volatile uint8_t uart_rx_head = 0;
static volatile uint8_t uart_rx_tail = 0;

// =============================================================================
// UART DRIVER FUNCTIONS
// =============================================================================

void UART_Init()
{
    /* Set baud rate */
    UBRR0H = (unsigned char)(MYUBRR >> 8);
    UBRR0L = (unsigned char)(MYUBRR);

    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    UCSR0C = (1 << URSEL0) | (1 << UCSZ01) | (1 << UCSZ00);
    sei();
}

void UART_Transmit(unsigned char data)
{
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

// Non-blocking receive - checks if data is available
unsigned char UART_DataAvailable(void)
{
    return (uart_rx_head != uart_rx_tail);
}

// Get one character from buffer
unsigned char UART_Receive(void)
{
    // Wait until data is available
    while (uart_rx_head == uart_rx_tail);
    
    unsigned char data = uart_rx_buffer[uart_rx_tail];
    uart_rx_tail = (uart_rx_tail + 1) % UART_RX_BUFFER_SIZE;
    
    return data;
}


void UART_Transmit_String(char *String)
{
    while (*String)
    {
        UART_Transmit(*String);
        String++;
    }
}

// Clear the receive buffer
void UART_Flush_RX_Buffer(void)
{
    uart_rx_head = 0;
    uart_rx_tail = 0;
}

// =============================================================================
// UART RX INTERRUPT SERVICE ROUTINE
// =============================================================================

ISR(USART0_RXC_vect)
{
    unsigned char data = UDR0;
    uint8_t next_head = (uart_rx_head + 1) % UART_RX_BUFFER_SIZE;
    
    // Only store data if buffer is not full
    if (next_head != uart_rx_tail)
    {
        uart_rx_buffer[uart_rx_head] = data;
        uart_rx_head = next_head;
    }
}

// =============================================================================
// UART & STDIO COUPLING
// =============================================================================

static int UART_Putchar(char c, FILE *stream)
{
    if (c == '\n')
    {
        UART_Putchar('\r', stream);
    }
    UART_Transmit(c);
    return 0;
}

static int UART_Getchar(FILE *stream)
{
    return UART_Receive();
}

FILE uart_output = FDEV_SETUP_STREAM(UART_Putchar, NULL, _FDEV_SETUP_WRITE);

FILE uart_input = FDEV_SETUP_STREAM(NULL, UART_Getchar, _FDEV_SETUP_READ);

int UART_Stdio_Init()
{
    stdout = &uart_output;
    stdin = &uart_input;
    return 0;
}
