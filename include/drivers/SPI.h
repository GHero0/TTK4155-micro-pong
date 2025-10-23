#ifndef SPI_H
#define SPI_H

#include <avr/io.h>

#define SS1 PB4
#define SS2 PB3
#define MOSI PB5
#define MISO PB6
#define SCK PB7

void SPI_Init(void);
void SPI_Select_Slave(unsigned char slave);
void SPI_Write_byte(unsigned char data);
unsigned char SPI_Read_byte(void);
#endif