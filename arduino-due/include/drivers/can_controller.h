/*
 * can_controller.h
 *
 * Author: Gustav O. Often and Eivind H. J�lsgard
 *
 * For use in TTK4155 Embedded and Industrial Computer Systems Design
 * NTNU - Norwegian University of Science and Technology
 *
 */ 


#ifndef CAN_CONTROLLER_H_
#define CAN_CONTROLLER_H_

#include "types.h"

#include <stdint.h>

uint8_t can_init_def_tx_rx_mb(uint32_t can_br);
uint8_t can_init(uint32_t can_br, uint8_t num_tx_mb, uint8_t num_rx_mb);

uint8_t can_send(CANMessage* can_msg, uint8_t mailbox_id);
uint8_t can_receive(CANMessage* can_msg, uint8_t mailbox_id);

// CAN bit timing configuration parameters
#define PHASE2 0x05
#define PHASE1 0x06
#define PROPAG 0x02
#define SJW    0x00
#define BRP    0x29
#define SMP    0x00

#define CAN_BR_VALUE ((PHASE2 << 0) | (PHASE1 << 4) | (PROPAG << 8) | (SJW << 12) | (BRP << 16) | (SMP << 24))

#endif /* CAN_CONTROLLER_H_ */