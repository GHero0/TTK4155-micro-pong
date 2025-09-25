#include "drivers/SPI.h"
#include "global.h"
#include <stdio.h>


void SPI_Init(void)
{
    // Set MOSI, SCK, SS1 as Output
    DDRB = (1 << MOSI) | (1 << SCK) | (1 << SS1);
    // Enable SPI, Set as Master
    SPCR = (1 << SPE) | (1 << MSTR);
}

void SPI_Select_Slave(unsigned char slave)
{
    if (slave == 1) {
        PORTB &= ~(1 << SS1); // Set SS1 low to select slave 1
    } else {
        PORTB |= (1 << SS1);  // Set SS1 high to deselect slave 1
    }
}

void SPI_Write_byte(unsigned char data)
{
    SPDR = data;
    while (!(SPSR & (1 << SPIF)));
    // Wait until transmission complete
}

unsigned char SPI_Read_byte(void)
{
    SPDR = 0xFF; // Send dummy byte to generate clock
    while (!(SPSR & (1 << SPIF)));
    // Wait until reception complete
    return SPDR;
}


