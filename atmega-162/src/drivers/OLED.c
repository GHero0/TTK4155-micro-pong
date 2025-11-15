/**
 * @file OLED.c
 * @brief OLED Driver and Frame Buffer Implementation
 */

// =============================================================================
// INCLUDES
// =============================================================================

// Personal headers
#include "drivers/SPI.h"
#include "config.h"
#include "drivers/OLED.h"

// Libraries
#include <util/delay.h>
#include <string.h>

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

// OLED frame buffer
unsigned char *volatile framebuffer = (unsigned char *)0x1400;
unsigned char *volatile doublebuffer = (unsigned char *)0x1800;
// Pointer aliases for convenience
unsigned char *volatile current_buffer;
unsigned char *volatile screen_buffer;

// =============================================================================
// DEFINES
// =============================================================================

#define OLED_DISP_DC_Data() (PORTB |= (1 << DISP_DC))     // DATA
#define OLED_DISP_DC_Command() (PORTB &= ~(1 << DISP_DC)) // CMD

// =============================================================================
// OLED FUNCTIONS
// =============================================================================

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

void FrameBuffer_Init()
{
    current_buffer = doublebuffer;
    screen_buffer = framebuffer;
    FrameBuffer_Clear();
    FrameBuffer_Swap();
    FrameBuffer_Clear();
    FrameBuffer_Push();
}

void FrameBuffer_Swap()
{
    unsigned char *volatile tmp = screen_buffer;
    screen_buffer = current_buffer;
    current_buffer = tmp;
}

void FrameBuffer_Clear()
{
    memset((unsigned char *)current_buffer, 0, 1024);
}

static inline __attribute__((always_inline)) unsigned char pack_bits(
    unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3,
    unsigned char b4, unsigned char b5, unsigned char b6, unsigned char b7,
    const unsigned char bit_pos)
{
    unsigned char result;
    __asm__ volatile(
        "clr %0\n\t"
        "sbrc %1, %9\n\t"
        "ori %0, 0x01\n\t"
        "sbrc %2, %9\n\t"
        "ori %0, 0x02\n\t"
        "sbrc %3, %9\n\t"
        "ori %0, 0x04\n\t"
        "sbrc %4, %9\n\t"
        "ori %0, 0x08\n\t"
        "sbrc %5, %9\n\t"
        "ori %0, 0x10\n\t"
        "sbrc %6, %9\n\t"
        "ori %0, 0x20\n\t"
        "sbrc %7, %9\n\t"
        "ori %0, 0x40\n\t"
        "sbrc %8, %9\n\t"
        "ori %0, 0x80\n\t"
        : "=r"(result)
        : "r"(b0), "r"(b1), "r"(b2), "r"(b3),
          "r"(b4), "r"(b5), "r"(b6), "r"(b7),
          "I"(bit_pos));
    return result;
}

void FrameBuffer_Push(void)
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
    const unsigned char *fb = screen_buffer;

    for (unsigned char p = 0; p < 8; p++)
    {
        // Precompute row pointers for this page using bit shifts instead of *16
        const unsigned char *base = fb + (p << 7); // (p * 8 * 16) = p << 7

        const unsigned char *row0 = base + (0 << 4);
        const unsigned char *row1 = base + (1 << 4);
        const unsigned char *row2 = base + (2 << 4);
        const unsigned char *row3 = base + (3 << 4);
        const unsigned char *row4 = base + (4 << 4);
        const unsigned char *row5 = base + (5 << 4);
        const unsigned char *row6 = base + (6 << 4);
        const unsigned char *row7 = base + (7 << 4);

        for (unsigned char i = 0; i < 16; i++)
        {
            unsigned char b0 = *row0++;
            unsigned char b1 = *row1++;
            unsigned char b2 = *row2++;
            unsigned char b3 = *row3++;
            unsigned char b4 = *row4++;
            unsigned char b5 = *row5++;
            unsigned char b6 = *row6++;
            unsigned char b7 = *row7++;

            // Pack all 8 bits using assembly
            SPI_Write_byte(pack_bits(b0, b1, b2, b3, b4, b5, b6, b7, 7));
            SPI_Write_byte(pack_bits(b0, b1, b2, b3, b4, b5, b6, b7, 6));
            SPI_Write_byte(pack_bits(b0, b1, b2, b3, b4, b5, b6, b7, 5));
            SPI_Write_byte(pack_bits(b0, b1, b2, b3, b4, b5, b6, b7, 4));
            SPI_Write_byte(pack_bits(b0, b1, b2, b3, b4, b5, b6, b7, 3));
            SPI_Write_byte(pack_bits(b0, b1, b2, b3, b4, b5, b6, b7, 2));
            SPI_Write_byte(pack_bits(b0, b1, b2, b3, b4, b5, b6, b7, 1));
            SPI_Write_byte(pack_bits(b0, b1, b2, b3, b4, b5, b6, b7, 0));
        }
    }

    SPI_Select_Slave(0);
}