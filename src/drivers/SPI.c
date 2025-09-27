#include "drivers/SPI.h"
#include "global.h"
#include <stdio.h>


void SPI_Init(void)
{
    // Set MOSI, SCK, SS1 as Output
    DDRB |= (1 << MOSI) | (1 << SCK);
    SPSR |= (1 << SPI2X);
    // Enable SPI, Set as Master
    SPCR = (1 << SPE) | (1 << MSTR);
}

void SPI_Select_Slave(unsigned char slave)
{
    PORTB |= (1 << SS1) | (1 << SS2); // deselect all
    if (slave == 1) PORTB &= ~(1 << SS1);
    if (slave == 2) PORTB &= ~(1 << SS2);
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


