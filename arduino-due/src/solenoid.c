#include "solenoid.h"
#include <sam.h>

void Solenoid_Init(void)
{
    PMC->PMC_PCER0 |= (1 << ID_PIOD);
    PIOD->PIO_PER  |= PIO_PER_P7;
    PIOD->PIO_OER  |= PIO_OER_P7;
}

void Solenoid_Update(uint8_t status)
{
    if (status) {
        PIOD->PIO_SODR = PIO_SODR_P7;
    } else {
        PIOD->PIO_CODR = PIO_CODR_P7;
    }
}
