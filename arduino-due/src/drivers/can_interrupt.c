#include "types.h"
#include "drivers/can_interrupt.h"
#include "drivers/can_controller.h"

#include <sam.h>
#include <stdio.h>

volatile char Flag_CAN_MB1 = 0;
volatile char Flag_CAN_MB2 = 0;
volatile CANMessage mb1_buffer;
volatile CANMessage mb2_buffer;

void CAN0_Handler(void)
{
    uint8_t status = CAN0->CAN_SR;

    // Check RX Mailbox 1
    if (status & CAN_SR_MB1)
    {
        can_receive((CANMessage *)&mb1_buffer, 1);

        Flag_CAN_MB1 = 1;

        // Re-arm mailbox properly for next message 
        CAN0->CAN_MB[1].CAN_MMR = CAN_MMR_MOT_MB_RX;
        CAN0->CAN_MB[1].CAN_MCR |= CAN_MCR_MTCR;
    }

    // Check RX Mailbox 2
    if (status & CAN_SR_MB2)
    {
        can_receive((CANMessage *)&mb2_buffer, 2);
        
		Flag_CAN_MB2 = 1;

        CAN0->CAN_MB[2].CAN_MMR = CAN_MMR_MOT_MB_RX;
        CAN0->CAN_MB[2].CAN_MCR |= CAN_MCR_MTCR;
    }

    NVIC_ClearPendingIRQ(ID_CAN0);
}
