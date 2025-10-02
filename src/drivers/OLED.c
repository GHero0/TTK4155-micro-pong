#include "drivers/SPI.h"
#include "drivers/OLED.h"
#include "fonts.h"
#include "global.h"

#include <stdio.h>
#include <util/delay.h>
#include <string.h>


#define OLED_DISP_DC_Data()    (PORTB |= (1 << DISP_DC))     // DATA
#define OLED_DISP_DC_Command() (PORTB &= ~(1 << DISP_DC))    // CMD

void OLED_Init(void)
{
    // Set outputs
    PORTB |= (1 << SS2) | (1 << PB1); // CS high, RES high
    PORTB &= ~(1 << DISP_DC);         // D/C = 0
    DDRB |= (1 << DISP_DC) | (1 << PB1) | (1 << SS2);

    // Hardware reset
    PORTB &= ~(1 << PB1);
    _delay_ms(1);
    PORTB |= (1 << PB1);
    _delay_ms(1);

    SPI_Init();
    SPI_Select_Slave(2);
    _delay_us(40);

    // OLED init commands
    OLED_DISP_DC_Command();
    SPI_Write_byte(0xAE); // Display OFF

    SPI_Write_byte(0xD5);
    SPI_Write_byte(0x00); // Clock divide ratio
    SPI_Write_byte(0xA8);
    SPI_Write_byte(0x3F); // Multiplex 1/64
    SPI_Write_byte(0xD3);
    SPI_Write_byte(0x00); // Display offset
    SPI_Write_byte(0x40); // Start line
    SPI_Write_byte(0xA1); // Segment remap
    SPI_Write_byte(0xC8); // COM scan direction
    SPI_Write_byte(0xDA);
    SPI_Write_byte(0x12); // COM pins config
    SPI_Write_byte(0x81);
    SPI_Write_byte(0x7F); // Contrast
    SPI_Write_byte(0xA4); // Resume RAM display
    SPI_Write_byte(0xA6); // Normal display
    SPI_Write_byte(0xD9);
    SPI_Write_byte(0x11); // Pre-charge
    SPI_Write_byte(0xDB);
    SPI_Write_byte(0x20); // VCOMH
    SPI_Write_byte(0xAF); // Display ON
    SPI_Write_byte(0x20);
    SPI_Write_byte(0x00); // Horizontal addressing

    SPI_Select_Slave(0);
}
// =============================================
//              OLED Frame System
// ---------------------------------------------
// "Everything should be made as simple 
// as possible, but not simpler".
// 
// Hours have been spent optimizing this piece 
// of code. Carefully think before modifying ... 
// =============================================

void FrameBufferInit()
{
    current_buffer = doublebuffer;
    screen_buffer  = framebuffer;
}

void FrameBufferSwap() 
{
    unsigned char * volatile tmp = screen_buffer;
    screen_buffer  = current_buffer;
    current_buffer = tmp;
}

void FrameBufferClear()
{
    memset((unsigned char *)current_buffer, 0, 1024);
}


void FrameBufferPush()
{
    OLED_DISP_DC_Command();
    SPI_Select_Slave(2);
    
    SPI_Write_byte(0x21);
    SPI_Write_byte(0x00);
    SPI_Write_byte(0x7F);
    SPI_Write_byte(0x22);
    SPI_Write_byte(0x00);
    SPI_Write_byte(0x07);

    OLED_DISP_DC_Data();
    
    volatile unsigned char *fb = screen_buffer;  // Use screen_buffer, not framebuffer!
    
    for (unsigned char p = 0; p < 8; p++)
    {
        unsigned int base_row = p << 3;
        
        for (unsigned char i = 0; i < 16; i++)
        {
            // Cache the 8 row addresses
            unsigned int r0 = i + ((base_row + 0) << 4);
            unsigned int r1 = i + ((base_row + 1) << 4);
            unsigned int r2 = i + ((base_row + 2) << 4);
            unsigned int r3 = i + ((base_row + 3) << 4);
            unsigned int r4 = i + ((base_row + 4) << 4);
            unsigned int r5 = i + ((base_row + 5) << 4);
            unsigned int r6 = i + ((base_row + 6) << 4);
            unsigned int r7 = i + ((base_row + 7) << 4);
            
            // Load all 8 bytes at once
            unsigned char b0 = fb[r0];
            unsigned char b1 = fb[r1];
            unsigned char b2 = fb[r2];
            unsigned char b3 = fb[r3];
            unsigned char b4 = fb[r4];
            unsigned char b5 = fb[r5];
            unsigned char b6 = fb[r6];
            unsigned char b7 = fb[r7];
            
            // Pack bit 7
            SPI_Write_byte(((b0 & 0x80) >> 7) | ((b1 & 0x80) >> 6) | ((b2 & 0x80) >> 5) | 
                          ((b3 & 0x80) >> 4) | ((b4 & 0x80) >> 3) | ((b5 & 0x80) >> 2) | 
                          ((b6 & 0x80) >> 1) | ((b7 & 0x80) >> 0));
            
            // Pack bit 6
            SPI_Write_byte(((b0 & 0x40) >> 6) | ((b1 & 0x40) >> 5) | ((b2 & 0x40) >> 4) | 
                          ((b3 & 0x40) >> 3) | ((b4 & 0x40) >> 2) | ((b5 & 0x40) >> 1) | 
                          ((b6 & 0x40) >> 0) | ((b7 & 0x40) << 1));
            
            // Pack bit 5
            SPI_Write_byte(((b0 & 0x20) >> 5) | ((b1 & 0x20) >> 4) | ((b2 & 0x20) >> 3) | 
                          ((b3 & 0x20) >> 2) | ((b4 & 0x20) >> 1) | ((b5 & 0x20) >> 0) | 
                          ((b6 & 0x20) << 1) | ((b7 & 0x20) << 2));
            
            // Pack bit 4
            SPI_Write_byte(((b0 & 0x10) >> 4) | ((b1 & 0x10) >> 3) | ((b2 & 0x10) >> 2) | 
                          ((b3 & 0x10) >> 1) | ((b4 & 0x10) >> 0) | ((b5 & 0x10) << 1) | 
                          ((b6 & 0x10) << 2) | ((b7 & 0x10) << 3));
            
            // Pack bit 3
            SPI_Write_byte(((b0 & 0x08) >> 3) | ((b1 & 0x08) >> 2) | ((b2 & 0x08) >> 1) | 
                          ((b3 & 0x08) >> 0) | ((b4 & 0x08) << 1) | ((b5 & 0x08) << 2) | 
                          ((b6 & 0x08) << 3) | ((b7 & 0x08) << 4));
            
            // Pack bit 2
            SPI_Write_byte(((b0 & 0x04) >> 2) | ((b1 & 0x04) >> 1) | ((b2 & 0x04) >> 0) | 
                          ((b3 & 0x04) << 1) | ((b4 & 0x04) << 2) | ((b5 & 0x04) << 3) | 
                          ((b6 & 0x04) << 4) | ((b7 & 0x04) << 5));
            
            // Pack bit 1
            SPI_Write_byte(((b0 & 0x02) >> 1) | ((b1 & 0x02) >> 0) | ((b2 & 0x02) << 1) | 
                          ((b3 & 0x02) << 2) | ((b4 & 0x02) << 3) | ((b5 & 0x02) << 4) | 
                          ((b6 & 0x02) << 5) | ((b7 & 0x02) << 6));
            
            // Pack bit 0
            SPI_Write_byte(((b0 & 0x01) >> 0) | ((b1 & 0x01) << 1) | ((b2 & 0x01) << 2) | 
                          ((b3 & 0x01) << 3) | ((b4 & 0x01) << 4) | ((b5 & 0x01) << 5) | 
                          ((b6 & 0x01) << 6) | ((b7 & 0x01) << 7));
        }
    }
    SPI_Select_Slave(0);
}   
// ----------
// Older code
// ----------
// void FrameBufferPush()
// {
//     OLED_DISP_DC_Command();
//     SPI_Select_Slave(2);
//     DDRB |= (1 << PB0);
//     PORTB ^= (1 << PB0);
//     // Set column/page addressing
//     SPI_Write_byte(0x21); // column
//     SPI_Write_byte(0x00);
//     SPI_Write_byte(0x7F);
//     SPI_Write_byte(0x22); // page
//     SPI_Write_byte(0x00);
//     SPI_Write_byte(0x07);

//     OLED_DISP_DC_Data();
    
//     unsigned char packed_data_0 = 0;
//     unsigned char packed_data_1 = 0;
//     unsigned char packed_data_2 = 0;
//     unsigned char packed_data_3 = 0;
//     unsigned char packed_data_4 = 0;
//     unsigned char packed_data_5 = 0;
//     unsigned char packed_data_6 = 0;
//     unsigned char packed_data_7 = 0;

//     for (int p = 0; p < 8; p++)
//     {
//         int base_row = p * 8;  // Base row for this page
        
//         for (int i = 0; i < 16; i++)
//         {
//             // Process each of the 8 bits in this byte (8 columns)
//             packed_data_0 = (((framebuffer[i + 16 * (base_row + 0)] & 0x80) >> 7) | 
//                              ((framebuffer[i + 16 * (base_row + 1)] & 0x80) >> 6) | 
//                              ((framebuffer[i + 16 * (base_row + 2)] & 0x80) >> 5) | 
//                              ((framebuffer[i + 16 * (base_row + 3)] & 0x80) >> 4) | 
//                              ((framebuffer[i + 16 * (base_row + 4)] & 0x80) >> 3) | 
//                              ((framebuffer[i + 16 * (base_row + 5)] & 0x80) >> 2) | 
//                              ((framebuffer[i + 16 * (base_row + 6)] & 0x80) >> 1) | 
//                              ((framebuffer[i + 16 * (base_row + 7)] & 0x80) >> 0)   
//             );

//             packed_data_1 = (((framebuffer[i + 16 * (base_row + 0)] & 0x40) >> 6) | 
//                              ((framebuffer[i + 16 * (base_row + 1)] & 0x40) >> 5) | 
//                              ((framebuffer[i + 16 * (base_row + 2)] & 0x40) >> 4) | 
//                              ((framebuffer[i + 16 * (base_row + 3)] & 0x40) >> 3) | 
//                              ((framebuffer[i + 16 * (base_row + 4)] & 0x40) >> 2) | 
//                              ((framebuffer[i + 16 * (base_row + 5)] & 0x40) >> 1) | 
//                              ((framebuffer[i + 16 * (base_row + 6)] & 0x40) >> 0) | 
//                              ((framebuffer[i + 16 * (base_row + 7)] & 0x40) << 1)   
//             );

//             packed_data_2 = (((framebuffer[i + 16 * (base_row + 0)] & 0x20) >> 5) | 
//                              ((framebuffer[i + 16 * (base_row + 1)] & 0x20) >> 4) | 
//                              ((framebuffer[i + 16 * (base_row + 2)] & 0x20) >> 3) | 
//                              ((framebuffer[i + 16 * (base_row + 3)] & 0x20) >> 2) | 
//                              ((framebuffer[i + 16 * (base_row + 4)] & 0x20) >> 1) | 
//                              ((framebuffer[i + 16 * (base_row + 5)] & 0x20) >> 0) | 
//                              ((framebuffer[i + 16 * (base_row + 6)] & 0x20) << 1) | 
//                              ((framebuffer[i + 16 * (base_row + 7)] & 0x20) << 2)   
//             );

//             packed_data_3 = (((framebuffer[i + 16 * (base_row + 0)] & 0x10) >> 4) | 
//                              ((framebuffer[i + 16 * (base_row + 1)] & 0x10) >> 3) | 
//                              ((framebuffer[i + 16 * (base_row + 2)] & 0x10) >> 2) | 
//                              ((framebuffer[i + 16 * (base_row + 3)] & 0x10) >> 1) | 
//                              ((framebuffer[i + 16 * (base_row + 4)] & 0x10) >> 0) | 
//                              ((framebuffer[i + 16 * (base_row + 5)] & 0x10) << 1) | 
//                              ((framebuffer[i + 16 * (base_row + 6)] & 0x10) << 2) | 
//                              ((framebuffer[i + 16 * (base_row + 7)] & 0x10) << 3)   
//             );

//             packed_data_4 = (((framebuffer[i + 16 * (base_row + 0)] & 0x08) >> 3) | 
//                              ((framebuffer[i + 16 * (base_row + 1)] & 0x08) >> 2) | 
//                              ((framebuffer[i + 16 * (base_row + 2)] & 0x08) >> 1) | 
//                              ((framebuffer[i + 16 * (base_row + 3)] & 0x08) >> 0) | 
//                              ((framebuffer[i + 16 * (base_row + 4)] & 0x08) << 1) | 
//                              ((framebuffer[i + 16 * (base_row + 5)] & 0x08) << 2) | 
//                              ((framebuffer[i + 16 * (base_row + 6)] & 0x08) << 3) | 
//                              ((framebuffer[i + 16 * (base_row + 7)] & 0x08) << 4)   
//             );

//             packed_data_5 = (((framebuffer[i + 16 * (base_row + 0)] & 0x04) >> 2) | 
//                              ((framebuffer[i + 16 * (base_row + 1)] & 0x04) >> 1) | 
//                              ((framebuffer[i + 16 * (base_row + 2)] & 0x04) >> 0) | 
//                              ((framebuffer[i + 16 * (base_row + 3)] & 0x04) << 1) | 
//                              ((framebuffer[i + 16 * (base_row + 4)] & 0x04) << 2) | 
//                              ((framebuffer[i + 16 * (base_row + 5)] & 0x04) << 3) | 
//                              ((framebuffer[i + 16 * (base_row + 6)] & 0x04) << 4) | 
//                              ((framebuffer[i + 16 * (base_row + 7)] & 0x04) << 5)   
//             );

//             packed_data_6 = (((framebuffer[i + 16 * (base_row + 0)] & 0x02) >> 1) | 
//                              ((framebuffer[i + 16 * (base_row + 1)] & 0x02) >> 0) | 
//                              ((framebuffer[i + 16 * (base_row + 2)] & 0x02) << 1) | 
//                              ((framebuffer[i + 16 * (base_row + 3)] & 0x02) << 2) | 
//                              ((framebuffer[i + 16 * (base_row + 4)] & 0x02) << 3) | 
//                              ((framebuffer[i + 16 * (base_row + 5)] & 0x02) << 4) | 
//                              ((framebuffer[i + 16 * (base_row + 6)] & 0x02) << 5) | 
//                              ((framebuffer[i + 16 * (base_row + 7)] & 0x02) << 6)   
//             );

//             packed_data_7 = (((framebuffer[i + 16 * (base_row + 0)] & 0x01) >> 0) | 
//                              ((framebuffer[i + 16 * (base_row + 1)] & 0x01) << 1) | 
//                              ((framebuffer[i + 16 * (base_row + 2)] & 0x01) << 2) | 
//                              ((framebuffer[i + 16 * (base_row + 3)] & 0x01) << 3) | 
//                              ((framebuffer[i + 16 * (base_row + 4)] & 0x01) << 4) | 
//                              ((framebuffer[i + 16 * (base_row + 5)] & 0x01) << 5) | 
//                              ((framebuffer[i + 16 * (base_row + 6)] & 0x01) << 6) | 
//                              ((framebuffer[i + 16 * (base_row + 7)] & 0x01) << 7)   
//             );
            
//             SPI_Write_byte(packed_data_0);
//             SPI_Write_byte(packed_data_1);
//             SPI_Write_byte(packed_data_2);
//             SPI_Write_byte(packed_data_3);
//             SPI_Write_byte(packed_data_4);
//             SPI_Write_byte(packed_data_5);
//             SPI_Write_byte(packed_data_6);
//             SPI_Write_byte(packed_data_7);
//         }
//     }

//     SPI_Select_Slave(0);
// }
