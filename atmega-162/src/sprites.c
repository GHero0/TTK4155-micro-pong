/**
 * @file sprites.c
 * @brief Sprites/tiles handling implementation and 
 * some usefull print functions for the screen
 */

#include "sprites.h"
#include "drivers/OLED.h"
#include "images.h"

#include <avr/pgmspace.h>

/*
* ==============================================================================
* Sprites / Tile System
* ------------------------------------------------------------------------------
* "Innovation with Limited Resources". 
* Graphics inspired by old Gameboy, tiling system.
*
* Hours have been spent optimizing this piece of code. 
* Carefully think before modifying ...
* ==============================================================================
*/

/*
* ==============================================================================
* Tilemap and Tile Transformations
* ------------------------------------------------------------------------------
* To save space we will use a tilemap for drawing onto the screen. The tilemap 
* is an array of all the tiles and it is stored in Flash memory.
*
* To spare as much space of Flash as possible, we shall use basic 
* transformations to multiply our avaiables tiles in memory.
* We have Sym(): Horizontal & Vertical Flip and 180° rotation
* ==============================================================================
*/

/*
* ==============================================================================
* Tile Formats 1bpp and 2bpp 
* ------------------------------------------------------------------------------
* 2bpp: Allow for more flexibility but takes more space
*   0b00 => Black
*   0b01 => XOR
*   0b10 => Transparent
*   0b11 => White
*
* 1bpp: Takes less space but can only be using carefully or for background
*   0b0 => Black
*   0b1 => White
* ==============================================================================
*/

static unsigned char tile_2bpp[16] = {
    0b11000000, 0b10000000,
    0b11000000, 0b01000000,
    0b01100000, 0b00100000,
    0b00110000, 0b00010000,
    0b00001100, 0b00001000,
    0b00001100, 0b00000100,
    0b00000011, 0b00000010,
    0b00000011, 0b00000001};

static unsigned char tile_1bpp[8] = {
    0b11000000,
    0b11000000,
    0b01100000,
    0b00110000,
    0b00001100,
    0b00000110,
    0b00000011,
    0b00000011};

#define INV_BITS_2bpp(b) (((b & 0x03) << 6) | ((b & 0x0C) << 2) | ((b & 0x30) >> 2) | ((b & 0xC0) >> 6))

inline void SYM_H_2bpp(void)
{
    register unsigned char tmp;
    
    for (unsigned char i = 0; i < 16; i += 2)
    {
        tmp = tile_2bpp[i];
        tile_2bpp[i] = INV_BITS_2bpp(tile_2bpp[i + 1]);
        tile_2bpp[i + 1] = INV_BITS_2bpp(tmp);
    }
}

inline void SYM_V_2bpp(void)
{
    register unsigned char tmp0, tmp1;
    
    // Loop through from outside to inside
    for (unsigned char i = 0; i < 8; i += 2)
    {
        unsigned char mirror_i = 14 - i;
        tmp0 = tile_2bpp[i];
        tmp1 = tile_2bpp[i + 1];
        tile_2bpp[i] = tile_2bpp[mirror_i];
        tile_2bpp[i + 1] = tile_2bpp[mirror_i + 1];
        tile_2bpp[mirror_i] = tmp0;
        tile_2bpp[mirror_i + 1] = tmp1;
    }
}

#define INV_BITS_1bpp(b) ( \
    ((b & 0x80) >> 7) |    \
    ((b & 0x40) >> 5) |    \
    ((b & 0x20) >> 3) |    \
    ((b & 0x10) >> 1) |    \
    ((b & 0x08) << 1) |    \
    ((b & 0x04) << 3) |    \
    ((b & 0x02) << 5) |    \
    ((b & 0x01) << 7))

inline void SYM_H_1bpp(void)
{
    tile_1bpp[0] = INV_BITS_1bpp(tile_1bpp[0]);
    tile_1bpp[1] = INV_BITS_1bpp(tile_1bpp[1]);
    tile_1bpp[2] = INV_BITS_1bpp(tile_1bpp[2]);
    tile_1bpp[3] = INV_BITS_1bpp(tile_1bpp[3]);
    tile_1bpp[4] = INV_BITS_1bpp(tile_1bpp[4]);
    tile_1bpp[5] = INV_BITS_1bpp(tile_1bpp[5]);
    tile_1bpp[6] = INV_BITS_1bpp(tile_1bpp[6]);
    tile_1bpp[7] = INV_BITS_1bpp(tile_1bpp[7]);
}

inline void SYM_V_1bpp(void)
{
    unsigned char tmp;

    tmp = tile_1bpp[0];
    tile_1bpp[0] = tile_1bpp[7];
    tile_1bpp[7] = tmp;
    tmp = tile_1bpp[1];
    tile_1bpp[1] = tile_1bpp[6];
    tile_1bpp[6] = tmp;
    tmp = tile_1bpp[2];
    tile_1bpp[2] = tile_1bpp[5];
    tile_1bpp[5] = tmp;
    tmp = tile_1bpp[3];
    tile_1bpp[3] = tile_1bpp[4];
    tile_1bpp[4] = tmp;
}

void draw_tile_2bpp(signed char X, signed char Y)
{
    unsigned char *fb = current_buffer;
    
    for (unsigned char r = 0; r < 8; r++)
    {
        signed char py = Y + r;
        if (py < 0 || py >= 64)
            continue;

        unsigned short row_offset = (unsigned char)py << 4;
        unsigned char tr_bytes[2] = {tile_2bpp[r << 1], tile_2bpp[(r << 1) + 1]};

        for (unsigned char i = 0; i < 8; i++)
        {
            unsigned char pix = (tr_bytes[i >> 2] >> ((3 - (i & 3)) << 1)) & 3;
            
            if (pix == 2)  // Transparent
                continue;
            
            signed char px = X + i;
            if ((unsigned char)px >= 128)
                continue;
            
            unsigned short idx = row_offset + ((unsigned char)px >> 3);
            unsigned char mask = 1 << (7 - (px & 7));
            
            fb[idx] = (pix == 3) ? (fb[idx] | mask) :
                     (pix == 0) ? (fb[idx] & ~mask) :
                                  (fb[idx] ^ mask);
        }
    }
}

void fetch_tile_from_tilemap_2bpp(unsigned short N)
{
    unsigned char base_Y = N >> 4;
    unsigned char base_X = N & 0x0F;

    unsigned short base_addr = (base_Y << 8) + (base_X << 1);
    const unsigned char *ptr = &tilemap_2bpp[base_addr];

    for (unsigned char i = 0; i < 8; i++)
    {
        tile_2bpp[i << 1] = pgm_read_byte(ptr);
        tile_2bpp[(i << 1) + 1] = pgm_read_byte(ptr + 1);
        ptr += 32;
    }
}

void draw_sprite_2bpp(Sprite Sp, signed char X, signed char Y)
{
    signed char base_y = Y;

    for (unsigned char row = 0; row < Sp.height_tile; row++)
    {
        int base_x = X;
        unsigned char row_offset = row * Sp.width_tile;

        for (unsigned char col = 0; col < Sp.width_tile; col++)
        {
            unsigned char tile_index = col + row_offset;

            // Read from PROGMEM
            unsigned char tile_num = pgm_read_byte(&Sp.sprite_tiles[tile_index]);
            unsigned char trans = pgm_read_byte(&Sp.trans_tiles[tile_index]);

            fetch_tile_from_tilemap_2bpp(tile_num);

            // Handle transformations
            if (trans & 0x01)
                SYM_H_2bpp(); // bit 0 = horizontal flip
            if (trans & 0x02)
                SYM_V_2bpp(); // bit 1 = vertical flip

            draw_tile_2bpp(base_x, base_y);

            base_x += 8;
        }
        base_y += 8;
    }
}

void fetch_tile_from_tilemap_1bpp(unsigned short N)
{
    unsigned short base_addr = ((N >> 4) << 7) + (N & 0x0F);
    const unsigned char *src = &tilemap_1bpp[base_addr];

    tile_1bpp[0] = pgm_read_byte(src);
    src += 16;
    tile_1bpp[1] = pgm_read_byte(src);
    src += 16;
    tile_1bpp[2] = pgm_read_byte(src);
    src += 16;
    tile_1bpp[3] = pgm_read_byte(src);
    src += 16;
    tile_1bpp[4] = pgm_read_byte(src);
    src += 16;
    tile_1bpp[5] = pgm_read_byte(src);
    src += 16;
    tile_1bpp[6] = pgm_read_byte(src);
    src += 16;
    tile_1bpp[7] = pgm_read_byte(src);
}

void draw_tile_1bpp(signed char X, signed char Y)
{
    signed char x_byte = X >> 3;
    if (x_byte < -1 || x_byte >= 16)
        return;

    unsigned char *fb = current_buffer;
    unsigned char x_bit = X & 7;

    if (x_bit == 0 && x_byte >= 0 && x_byte < 16 && Y >= 0 && Y <= 56)
    {
        unsigned short idx = ((unsigned short)Y << 4) + (unsigned char)x_byte;
        fb[idx] = tile_1bpp[0];
        fb[idx + 16] = tile_1bpp[1];
        fb[idx + 32] = tile_1bpp[2];
        fb[idx + 48] = tile_1bpp[3];
        fb[idx + 64] = tile_1bpp[4];
        fb[idx + 80] = tile_1bpp[5];
        fb[idx + 96] = tile_1bpp[6];
        fb[idx + 112] = tile_1bpp[7];
        return;
    }

    unsigned char lm = 0xFF << (8 - x_bit);
    unsigned char rm = 0xFF >> x_bit;
    unsigned char sr = x_bit;
    unsigned char sl = 8 - x_bit;

    for (unsigned char r = 0; r < 8; r++)
    {
        signed char py = Y + r;
        if (py < 0 || py >= 64)
            continue;

        unsigned short offs = ((unsigned short)py << 4);
        unsigned char row = tile_1bpp[r];
        unsigned char left = row >> sr;
        unsigned char right = row << sl;

        if (x_byte >= 0 && x_byte < 16)
        {
            unsigned short idx = offs + (unsigned char)x_byte;
            fb[idx] = (fb[idx] & lm) | left;
        }
        if (x_byte < 15)
        {
            unsigned short idx = offs + (unsigned char)(x_byte + 1);
            fb[idx] = (fb[idx] & rm) | right;
        }
    }
}

void draw_sprite_1bpp(Sprite Sp, signed char X, signed char Y)
{
    for (unsigned char row = 0; row < Sp.height_tile; row++)
    {
        for (unsigned char col = 0; col < Sp.width_tile; col++)
        {
            unsigned char tile_index = col + row * Sp.width_tile;

            // Read from PROGMEM!
            unsigned char tile_num = pgm_read_byte(&Sp.sprite_tiles[tile_index]);
            unsigned char trans = pgm_read_byte(&Sp.trans_tiles[tile_index]);

            fetch_tile_from_tilemap_1bpp(tile_num);

            switch (trans)
            {
            case 0: // normal draw
                draw_tile_1bpp(X + (col << 3), Y + (row << 3));
                break;

            case 1:
                SYM_H_1bpp();
                draw_tile_1bpp(X + (col << 3), Y + (row << 3));
                break;

            case 2:
                SYM_V_1bpp();
                draw_tile_1bpp(X + (col << 3), Y + (row << 3));
                break;

            case 3:
                SYM_H_1bpp();
                SYM_V_1bpp();
                draw_tile_1bpp(X + (col << 3), Y + (row << 3));
                break;

            default:
                draw_tile_1bpp(X + (col << 3), Y + (row << 3));
                break;
            }
        }
    }
}

void draw_tilemap_1bpp(void)
{
    for (unsigned char row = 0; row < 8; row++)
    {
        for (unsigned char col = 0; col < 16; col++)
        {
            unsigned char tile_index = col + row * 16;
            fetch_tile_from_tilemap_1bpp(tile_index);
            draw_tile_1bpp(col << 3, row << 3);
        }
    }
}

void draw_tilemap_2bpp(void)
{
    for (unsigned char row = 0; row < 4; row++)
    {
        for (unsigned char col = 0; col < 16; col++)
        {
            unsigned char tile_index = col + row * 16;
            fetch_tile_from_tilemap_2bpp(tile_index);
            draw_tile_2bpp(col << 3, row << 3);
        }
    }
}


void draw_rectangle(int X, int Y, unsigned char width, unsigned char height) {
    if (X >= 128 || Y >= 64 || X + width <= 0 || Y + height <= 0) return;
    
    signed char x1 = X;
    signed char y1 = Y;
    signed char x2 = X + width - 1;
    signed char y2 = Y + height - 1;
    
    char draw_top = (Y >= 0 && Y < 64);
    char draw_bottom = (y2 >= 0 && y2 < 64 && y2 != y1);
    char draw_left = (X >= 0 && X < 128);
    char draw_right = (x2 >= 0 && x2 < 128 && x2 != x1);
    
    if (draw_top && width > 2) {
        draw_line(x1 + 1, y1, width - 2, 0, 0);
    }
    
    if (draw_bottom && width > 2) {
        draw_line(x1 + 1, y2, width - 2, 0, 0);
    }
    
    if (draw_left && height > 2) {
        draw_line(x1, y1 + 1, height - 2, 1, 0);
    }
    
    if (draw_right && height > 2) {
        draw_line(x2, y1 + 1, height - 2, 1, 0);
    }
}

void draw_line(signed char x, signed char y, unsigned char length, char direction, char stride) {
    if (direction == 0) {
        // Horizontal line
        if (y < 0 || y >= 64 || x >= 128) return;
        if (x < 0) {
            length += x;
            x = 0;
        }
        if (x + length > 128) length = 128 - x;
        if (length <= 0) return;
        
        unsigned char byte_pos = x >> 3;
        unsigned char bit_pos = x & 7;
        unsigned short base_idx = ((unsigned char)y << 4) + byte_pos;
        
        if (stride == 0) {
            // Optimized filled horizontal line
            signed char x_end = x + length - 1;
            unsigned char end_byte = x_end >> 3;
            unsigned char end_bit = x_end & 7;
            
            if (byte_pos == end_byte) {
                // Single byte
                current_buffer[base_idx] |= (0xFF >> bit_pos) & (0xFF << (7 - end_bit));
            } else {
                // Multiple bytes
                current_buffer[base_idx++] |= 0xFF >> bit_pos;
                while (++byte_pos < end_byte) {
                    current_buffer[base_idx++] = 0xFF;
                }
                current_buffer[base_idx] |= 0xFF << (7 - end_bit);
            }
        } else {
            // Strided horizontal
            for (unsigned char i = 0; i < length; i += stride + 1) {
                if ((x + i) >= 128) break;
                unsigned short idx = base_idx + ((x + i) >> 3) - byte_pos;
                current_buffer[idx] |= 0x80 >> ((x + i) & 7);
            }
        }
    } else {
        // Vertical line
        if (x < 0 || x >= 128 || y >= 64) return;
        if (y < 0) {
            length += y;
            y = 0;
        }
        if (y + length > 64) length = 64 - y;
        if (length <= 0) return;
        
        unsigned char bit_mask = 0x80 >> (x & 7);
        unsigned short idx = ((unsigned char)y << 4) + (x >> 3);
        unsigned char step = (stride == 0) ? 1 : stride + 1;
        unsigned short row_add = 16 * step;
        
        for (unsigned char i = 0; i < length; i += step) {
            current_buffer[idx] |= bit_mask;
            idx += row_add;
        }
    }
}

static void draw_char(char c, char X, char Y)
{
    if (c < 32 || c > 126)
        return; // Unsupported character
    if (c > 96 && c <= 122)
    {
        c -= 32;
    }
    if (c > 122)
    {
        c -= 26;
    }

    fetch_tile_from_tilemap_1bpp(c + 27);
    draw_tile_1bpp(X, Y);
}

void draw_string(const char *str, char X, char Y)
{
    char x = X;
    char y = Y;
    unsigned char c_pos;

    while (*str)
    {
        if (*str == '\n')
        {
            x = X;  // Reset to initial X
            y += 6; // Move down one character height
        }
        else
        {
            c_pos = *str;
            if (*str > 96)
                c_pos -= 32;
            draw_char(*str, x, y);
            x += pgm_read_byte(&font_spacing[c_pos - 32]); // Move right character width
        }
        str++;
    }
}
