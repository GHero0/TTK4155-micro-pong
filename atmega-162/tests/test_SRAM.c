#include "hardware.h"
#include "tests/test_SRAM.h"

#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

void test_SRAM(void)
{
    volatile char *ext_ram = (char *)0x1400; // Start address for the SRAM
    uint16_t ext_ram_size = 0xC00;
    uint16_t write_errors = 0;
    uint16_t retrieval_errors = 0;
    printf("Starting SRAM address test...\n");
    //  rand() stores some internal state, so calling this function in a loop will
    //  yield different seeds each time (unless srand() is called before this function)
    uint16_t seed = rand();
    // Write phase: Immediately check that the correct value was stored
    srand(seed);
    for (uint16_t i = 0; i < ext_ram_size; i++)
    {
        uint8_t some_value = rand();
        ext_ram[i] = some_value;
        uint8_t retreived_value = ext_ram[i];
        if (retreived_value != some_value)
        {
            printf("Write phase error: ext_ram[%4d] = %02X (should be %02X)\n", i, retreived_value, some_value);
            write_errors++;
        }
    }
    // Retrieval phase: Check that no values were changed during or after the write phase
    srand(seed);
    // reset the PRNG to the state it had before the write phase
    for (uint16_t i = 0; i < ext_ram_size; i++)
    {
        uint8_t some_value = rand();
        uint8_t retreived_value = ext_ram[i];
        if (retreived_value != some_value)
        {
            printf("Retrieval phase error: ext_ram[%4d] = %02X (should be %02X)\n", i, retreived_value, some_value);
            retrieval_errors++;
        }
    }
    printf("SRAM address test completed with \n%4d errors in write phase and \n%4d errors in retrieval phase\n\n", write_errors, retrieval_errors);
}

void test_address_decoder(void)
{
    printf("Starting Address decoder test ... \n\n");
    printf("Trying to address ADC ... \n");

    volatile char *ext_ram = (char *)0x0000; // Start address
    unsigned char t;

    for (t = 0; t < 10; t++)
    {
        ext_ram[0x13FF] = 11;
    }
    _delay_ms(5000);
    printf("Trying to address SRAM ... \n\n");
    for (t = 0; t < 100; t++)
    {
        ext_ram[0x1400] = 11;
    }

    printf("Address decoder test completed\n\n");
}