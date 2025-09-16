#include "tests/test_UART_driver.h"
#include "drivers/UART_driver.h"
#include <stdbool.h>
#include <string.h>

void test_UART_Transmit(){
    unsigned char i;
    
    for (i = 0; i < 10; i++)
    {
        USART_Transmit('f');
        USART_Transmit('\n');
        USART_Transmit('\r');
    }
    for (i = 0; i < 10; i++)
    {
        USART_Transmit_String("Hello World !");
        USART_Transmit('\n');
        USART_Transmit('\r');
    }
    for (i = 0; i < 10; i++)
    {
        printf("Test %d || 0x%02X\n", i, i);
    }
}

void test_UART_Receive(){
    unsigned char received_char;
    char received_buffer[5]; // Increased size for null terminator
    bool part1_end = false;
    bool part2_end = false;

    while (!(part1_end)){
        received_char = USART_Receive();
        printf("Received character: %c\n", received_char);
        if (received_char == 'f') part1_end = true;
    }

    while(!(part2_end)){
        scanf("%4s", received_buffer); // Read 4 chars + null terminator
        printf("Received string: %s\n", received_buffer);
        if (strcmp(received_buffer, "exit") == 0) part2_end = true;
    }
}
