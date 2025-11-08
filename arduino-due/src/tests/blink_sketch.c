/*
 * CFile1.c
 *
 * Created: 08.11.2025 13:15:48
 *  Author: flaucht
 */ 

#include "sam.h"
#include <stdio.h>
#include "time.h"

void blink_sketch_init(){	
	//enable output peripheral
	PMC->PMC_PCER0 |= (1 << ID_PIOD);
	// set as output
	PIOD->PIO_OER = PIO_OER_P7;
}



void blink_sketch(){
	printf("test\n");
	//PIOD->PIO_SODR = (1 << PIO_PD7);
	PIOD->PIO_SODR = PIO_SODR_P7;
	printf("after turning on\n");
	time_spinFor(msecs(1000));
	printf("after delay\n");
	//PIOD->PIO_CODR = (1 << PIO_PD7);
	PIOD->PIO_CODR = PIO_CODR_P7;
	time_spinFor(msecs(1000));
}