#ifndef TEST_SRAM_H
#define TEST_SRAM_H

#include "global.h"
// SRAM Tests
void test_SRAM(void);           // Checks if IC writes and reads correctly in external memory
void test_address_decoder(void);// Cycles between adressing ADC or SRAM just to verify NAND logic
#endif