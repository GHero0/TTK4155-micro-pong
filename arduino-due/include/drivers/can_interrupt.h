/*
 * can_interrupt.h
 *
 * Author: Gustav O. Often and Eivind H. J�lsgard
 *
 * For use in TTK4155 Embedded and Industrial Computer Systems Design
 * NTNU - Norwegian University of Science and Technology
 *
 */ 

#ifndef CAN_INTERRUPT_H_
#define CAN_INTERRUPT_H_

void CAN0_Handler       ( void );

extern volatile char Flag_CAN_MB1;
extern volatile char Flag_CAN_MB2;
extern volatile CANMessage mb1_buffer;
extern volatile CANMessage mb2_buffer;

#endif /* CAN_INTERRUPT_H_ */