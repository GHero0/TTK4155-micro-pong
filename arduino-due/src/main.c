#include "sam.h"
#include "stdio.h"
#include "time.h"
#include "uart.h"
#include "can_controller.h"
#include "can_interrupt.h"

// CAN bit timing configuration parameters
#define PHASE2 0x05  // Phase 2 segment (bits 2:0)
#define PHASE1 0x06  // Phase 1 segment (bits 6:4)
#define PROPAG 0x02  // Propagation segment (bits 10:8)
#define SJW    0x00  // Synchronization Jump Width (bits 13:12)
#define BRP    0x29  // Baud Rate Prescaler (bits 22:16)
#define SMP    0x00  // Sampling mode (bit 24): 0=once, 1=three times

// Manually construct CAN_BR register value with bit shifting
#define CAN_BR_VALUE ((PHASE2 << 0) | \
                      (PHASE1 << 4) | \
                      (PROPAG << 8) | \
                      (SJW << 12) | \
                      (BRP << 16) | \
                      (SMP << 24))


int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	
    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer

    /* UART (84MHz, 9600 Baudrate) */
    uart_init(84000000L, 9600);
	
    // Initialize CAN with default configuration:
    // - 1 TX mailbox (Mailbox 0)
    // - 2 RX mailboxes (Mailbox 1 and 2)
    // The CAN_BR_VALUE is written to the CAN0->CAN_BR register
    uint8_t can_status = can_init_def_tx_rx_mb(CAN_BR_VALUE);
    
    if (can_status != 0) {
        printf("CAN initialization failed!\n");
        while(1); // Halt on error
    }
    
    printf("CAN initialized successfully\n");
    
    int i = 0;
    
    while(1)
    {
        // printf("Sending CAN message %d\n", i);
        
        // // Create CAN message using the CAN_MESSAGE structure from can_controller.h
        // CAN_MESSAGE msg_sent;
        // msg_sent.id = 1;                // CAN message ID (11-bit standard or 29-bit extended)
        // msg_sent.data_length = 4;       // Send 8 bytes (maximum for CAN)
        
        // // Fill the data payload
        // msg_sent.data[0] = 0xAA;
        // msg_sent.data[1] = 0xBB;
        // msg_sent.data[2] = 0xCC;
        // msg_sent.data[3] = 0xDD;

        // // Send message using TX mailbox 0
        // // can_send returns 0 on success, 1 if mailbox is busy
        // uint8_t send_status = can_send(&msg_sent, 0);
        
        // if (send_status == 0) {
        //     printf("Message sent successfully\n");
        // } else {
        //     printf("Failed to send message (mailbox busy)\n");
        // }
        
        // // Wait 1 second before sending next message
        // time_spinFor(msecs(1000));
        // i++;

		printf("Receiving CAN message %d\n", i);
        
        // Create CAN message using the CAN_MESSAGE structure from can_controller.h
        CAN_MESSAGE msg_sent;
    
        // Send message using TX mailbox 0
        // can_send returns 0 on success, 1 if mailbox is busy
        uint8_t receive_status = can_receive(&msg_sent,0);
        
        if (receive_status == 0) {
            printf("Message received successfully\n");
        } else {
            printf("Failed to receive message (mailbox busy)\n");
        }
        
        // Wait 1 second before receiving next message
        // time_spinFor(msecs(1000));
        i++;
    }
}