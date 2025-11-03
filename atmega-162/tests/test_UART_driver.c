#include "tests/test_UART_driver.h"
#include "drivers/UART_driver.h"

#include <stdbool.h>
#include <string.h>
#include <stdio.h>

void test_UART_Transmit(){
    unsigned char i;
    
    for (i = 0; i < 10; i++)
    {
        UART_Transmit('f');
        UART_Transmit('\n');
        UART_Transmit('\r');
    }
    for (i = 0; i < 10; i++)
    {
        UART_Transmit_String("Hello World !");
        UART_Transmit('\n');
        UART_Transmit('\r');
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
        received_char = UART_Receive();
        printf("Received character: %c\n", received_char);
        if (received_char == 'f') part1_end = true;
    }

    while(!(part2_end)){
        if (fgets(received_buffer, sizeof(received_buffer), stdin) != NULL) {
            // Remove trailing newline if present
            size_t len = strlen(received_buffer);
            if (len > 0 && received_buffer[len - 1] == '\n') {
                received_buffer[len - 1] = '\0';
            }
        }
        printf("Received string: %s\n", received_buffer);
        if (strcmp(received_buffer, "exit") == 0) part2_end = true;
    }
}
