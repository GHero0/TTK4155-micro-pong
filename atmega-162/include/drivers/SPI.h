/**
 * @file SPI.h
 * @brief SPI prototypes
 */

#ifndef SPI_H
#define SPI_H

void SPI_Init(void);
void SPI_Select_Slave(unsigned char slave);
void SPI_Write_byte(unsigned char data);
unsigned char SPI_Read_byte(void);
#endif