#include "sprites.h"
#include "drivers/OLED.h"
#include "global.h"
#include "images.h"

#include <avr/pgmspace.h>


// =============================================
//           Sprites / Tile System
// ---------------------------------------------
// "Innovation with Limited Resources". Graphics
// inspired by old Gameboy, tiling system.
// 
// Hours have been spent optimizing this piece 
// of code. Carefully think before modifying ... 
// 
// Time spent developing and optimizing: 0 hours
// =============================================




// The tilemap is a representation of every tile.
// It is stored in Flash memory.

// What is a tile: It's 8px x 8px. Instead of 
// representing a pixel by 1 bit, we will 
// represent it on 2 bits. It's usefull for 
// sprites transparency & cool FX. 
//
// ----------------------------------
//   Format of pixel format in tile  
// ----------------------------------
// 0b00 => Black
// 0b01 => XOR
// 0b10 => Transparent
// 0b11 => White




// =============================================
//           Tile manipulation
// ---------------------------------------------
//
// To spare as much space of Flash as possible, 
// we shall use basic transformations to 
// multiply our avaiables tiles in memory.
//
// We have Sym(): Horizontal & Vertical Flip
// And also Rot(): For 90deg rotations
// We combine those to get the other variatons.
// So in total we have 8 variations per tile.
// 
// --------------------------------------------


// The tilemap is a representation of every tile.
// It is stored in Flash memory.

// What is a tile: It's 8px x 8px. Instead of 
// representing a pixel by 1 bit, we will 
// represent it on 2 bits. It's usefull for 
// sprites transparency & cool FX. 
//
// ----------------------------------
//   Format of pixel format in tile  
// ----------------------------------
// 0b00 => Black
// 0b01 => XOR
// 0b10 => Transparent
// 0b11 => White

unsigned char tile_2bpp[16] = {
    0b11000000, 0b10000000,
    0b11000000, 0b01000000,
    0b01100000, 0b00100000,
    0b00110000, 0b00010000,
    0b00001100, 0b00001000,
    0b00001100, 0b00000100,
    0b00000011, 0b00000010,
    0b00000011, 0b00000001
};

unsigned char tile_1bpp[8] = {
    0b11000000, 
    0b11000000, 
    0b01100000, 
    0b00110000, 
    0b00001100, 
    0b00000110, 
    0b00000011, 
    0b00000011
};



// =============================================
//           Tile manipulation
// ---------------------------------------------
//
// To spare as much space of Flash as possible, 
// we shall use basic transformations to 
// multiply our avaiables tiles in memory.
//
// We have Sym(): Horizontal & Vertical Flip
// And also Rot(): For 90deg rotations
// We combine those to get the other variatons.
// So in total we have 8 variations per tile.
// 
// --------------------------------------------

#define INV_BITS_2bpp(b)  (((b & 0x03) << 6) | ((b & 0x0C) << 2) | ((b & 0x30) >> 2) | ((b & 0xC0) >> 6))
#define SAFE_SHIFT(table, shift)  ((shift) >= 0 ? (table) << (shift) : (table) >> ((-(shift)) & 7))

static inline void SYM_H_2bpp(void)
{
    register unsigned char tmp;

    tmp = tile_2bpp[0]; tile_2bpp[0] = INV_BITS_2bpp(tile_2bpp[1]); tile_2bpp[1] = INV_BITS_2bpp(tmp);
    tmp = tile_2bpp[2]; tile_2bpp[2] = INV_BITS_2bpp(tile_2bpp[3]); tile_2bpp[3] = INV_BITS_2bpp(tmp);
    tmp = tile_2bpp[4]; tile_2bpp[4] = INV_BITS_2bpp(tile_2bpp[5]); tile_2bpp[5] = INV_BITS_2bpp(tmp);
    tmp = tile_2bpp[6]; tile_2bpp[6] = INV_BITS_2bpp(tile_2bpp[7]); tile_2bpp[7] = INV_BITS_2bpp(tmp);
    tmp = tile_2bpp[8]; tile_2bpp[8] = INV_BITS_2bpp(tile_2bpp[9]); tile_2bpp[9] = INV_BITS_2bpp(tmp);
    tmp = tile_2bpp[10]; tile_2bpp[10] = INV_BITS_2bpp(tile_2bpp[11]); tile_2bpp[11] = INV_BITS_2bpp(tmp);
    tmp = tile_2bpp[12]; tile_2bpp[12] = INV_BITS_2bpp(tile_2bpp[13]); tile_2bpp[13] = INV_BITS_2bpp(tmp);
    tmp = tile_2bpp[14]; tile_2bpp[14] = INV_BITS_2bpp(tile_2bpp[15]); tile_2bpp[15] = INV_BITS_2bpp(tmp);
}


static inline void SYM_V_2bpp(void)
{
    register unsigned char tmp0,tmp1;

    tmp0 = tile_2bpp[0]; tmp1 = tile_2bpp[1]; tile_2bpp[0] = tile_2bpp[14]; tile_2bpp[1] = tile_2bpp[15]; tile_2bpp[14] = tmp0; tile_2bpp[15] = tmp1;
    tmp0 = tile_2bpp[2]; tmp1 = tile_2bpp[3]; tile_2bpp[2] = tile_2bpp[12]; tile_2bpp[3] = tile_2bpp[13]; tile_2bpp[12] = tmp0; tile_2bpp[13] = tmp1;
    tmp0 = tile_2bpp[4]; tmp1 = tile_2bpp[5]; tile_2bpp[4] = tile_2bpp[10]; tile_2bpp[5] = tile_2bpp[11]; tile_2bpp[10] = tmp0; tile_2bpp[11] = tmp1;
    tmp0 = tile_2bpp[6]; tmp1 = tile_2bpp[7]; tile_2bpp[6] = tile_2bpp[8];  tile_2bpp[7] = tile_2bpp[9];  tile_2bpp[8]  = tmp0; tile_2bpp[9]  = tmp1;
}


#define INV_BITS_1bpp(b) ( \
    ((b & 0x80) >> 7) | \
    ((b & 0x40) >> 5) | \
    ((b & 0x20) >> 3) | \
    ((b & 0x10) >> 1) | \
    ((b & 0x08) << 1) | \
    ((b & 0x04) << 3) | \
    ((b & 0x02) << 5) | \
    ((b & 0x01) << 7) )

static void SYM_H_1bpp(void)
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

static void SYM_V_1bpp(void)
{
    unsigned char tmp;

    tmp = tile_1bpp[0]; tile_1bpp[0] = tile_1bpp[7]; tile_1bpp[7] = tmp;
    tmp = tile_1bpp[1]; tile_1bpp[1] = tile_1bpp[6]; tile_1bpp[6] = tmp;
    tmp = tile_1bpp[2]; tile_1bpp[2] = tile_1bpp[5]; tile_1bpp[5] = tmp;
    tmp = tile_1bpp[3]; tile_1bpp[3] = tile_1bpp[4]; tile_1bpp[4] = tmp;
}

// static inline void draw_tile_2bpp(int X, int Y, unsigned char warp) {
//     unsigned char *fb = current_buffer;
//     if (warp) {
//         // WARP MODE - modulo wrapping
//         unsigned char py_base = Y & 63;  // Hoist Y masking outside loop
        
//         for (unsigned char r = 0; r < 8; r++) {
//             unsigned char py = (py_base + r) & 63;
//             unsigned int row_offset = (unsigned int)py << 4;
            
//             unsigned char tr = tile_2bpp[r << 1];
//             unsigned char tr2 = tile_2bpp[(r << 1) + 1];
            
//             #define DRAW_PIXEL_WRAP(C, SHIFT, TREG) \
//                 { \
//                     unsigned char pix = ((TREG >> SHIFT) & 3); \
//                     if (pix != 2) { \
//                         unsigned char px = (X + C) & 127; \
//                         unsigned int idx = row_offset + (px >> 3); \
//                         unsigned char mask = 1 << (7 - (px & 7)); \
//                         if (pix == 3) fb[idx] |= mask; \
//                         else if (pix == 0) fb[idx] &= ~mask; \
//                         else fb[idx] ^= mask; \
//                     } \
//                 }
            
//             DRAW_PIXEL_WRAP(0, 6, tr)
//             DRAW_PIXEL_WRAP(1, 4, tr)
//             DRAW_PIXEL_WRAP(2, 2, tr)
//             DRAW_PIXEL_WRAP(3, 0, tr)
//             DRAW_PIXEL_WRAP(4, 6, tr2)
//             DRAW_PIXEL_WRAP(5, 4, tr2)
//             DRAW_PIXEL_WRAP(6, 2, tr2)
//             DRAW_PIXEL_WRAP(7, 0, tr2)
            
//             #undef DRAW_PIXEL_WRAP
//         }
//     } else {
//         // NON-WARP MODE - clipping
//         for (unsigned char r = 0; r < 8; r++) {
//             int py = Y + r;
//             if (py < 0 || py >= 64) continue;
            
//             unsigned int row_offset = (unsigned int)py << 4;
//             unsigned char tr = tile_2bpp[r << 1];
//             unsigned char tr2 = tile_2bpp[(r << 1) + 1];
            
//             #define DRAW_PIXEL_CLIP(C, SHIFT, TREG) \
//                 { \
//                     int px = X + C; \
//                     if ((unsigned int)px < 128) { \
//                         unsigned char pix = ((TREG >> SHIFT) & 3); \
//                         if (pix != 2) { \
//                             unsigned int idx = row_offset + (px >> 3); \
//                             unsigned char mask = 1 << (7 - (px & 7)); \
//                             if (pix == 3) current_buffer[idx] |= mask; \
//                             else if (pix == 0) current_buffer[idx] &= ~mask; \
//                             else current_buffer[idx] ^= mask; \
//                         } \
//                     } \
//                 }
            
//             DRAW_PIXEL_CLIP(0, 6, tr)
//             DRAW_PIXEL_CLIP(1, 4, tr)
//             DRAW_PIXEL_CLIP(2, 2, tr)
//             DRAW_PIXEL_CLIP(3, 0, tr)
//             DRAW_PIXEL_CLIP(4, 6, tr2)
//             DRAW_PIXEL_CLIP(5, 4, tr2)
//             DRAW_PIXEL_CLIP(6, 2, tr2)
//             DRAW_PIXEL_CLIP(7, 0, tr2)
            
//             #undef DRAW_PIXEL_CLIP
//         }
//     }
// }

static inline void draw_tile_2bpp(int X, int Y, unsigned char warp) {
    unsigned char *fb = current_buffer;
    
    if (warp) {
        // WARP MODE - modulo wrapping
        unsigned char py_base = Y & 63;
        
        for (unsigned char r = 0; r < 8; r++) {
            unsigned char py = (py_base + r) & 63;
            unsigned int row_offset = (unsigned int)py << 4;
            
            unsigned char tr = tile_2bpp[r << 1];
            unsigned char tr2 = tile_2bpp[(r << 1) + 1];
            
            // Pre-extract all 8 pixels (eliminates repeated shifting)
            unsigned char pix0 = (tr >> 6) & 3;
            unsigned char pix1 = (tr >> 4) & 3;
            unsigned char pix2 = (tr >> 2) & 3;
            unsigned char pix3 = tr & 3;
            unsigned char pix4 = (tr2 >> 6) & 3;
            unsigned char pix5 = (tr2 >> 4) & 3;
            unsigned char pix6 = (tr2 >> 2) & 3;
            unsigned char pix7 = tr2 & 3;
            
            // Draw pixel 0
            if (pix0 != 2) {
                unsigned char px = (X + 0) & 127;
                unsigned int idx = row_offset + (px >> 3);
                unsigned char mask = 1 << (7 - (px & 7));
                if (pix0 == 3) fb[idx] |= mask;
                else if (pix0 == 0) fb[idx] &= ~mask;
                else fb[idx] ^= mask;
            }
            
            // Draw pixel 1
            if (pix1 != 2) {
                unsigned char px = (X + 1) & 127;
                unsigned int idx = row_offset + (px >> 3);
                unsigned char mask = 1 << (7 - (px & 7));
                if (pix1 == 3) fb[idx] |= mask;
                else if (pix1 == 0) fb[idx] &= ~mask;
                else fb[idx] ^= mask;
            }
            
            // Draw pixel 2
            if (pix2 != 2) {
                unsigned char px = (X + 2) & 127;
                unsigned int idx = row_offset + (px >> 3);
                unsigned char mask = 1 << (7 - (px & 7));
                if (pix2 == 3) fb[idx] |= mask;
                else if (pix2 == 0) fb[idx] &= ~mask;
                else fb[idx] ^= mask;
            }
            
            // Draw pixel 3
            if (pix3 != 2) {
                unsigned char px = (X + 3) & 127;
                unsigned int idx = row_offset + (px >> 3);
                unsigned char mask = 1 << (7 - (px & 7));
                if (pix3 == 3) fb[idx] |= mask;
                else if (pix3 == 0) fb[idx] &= ~mask;
                else fb[idx] ^= mask;
            }
            
            // Draw pixel 4
            if (pix4 != 2) {
                unsigned char px = (X + 4) & 127;
                unsigned int idx = row_offset + (px >> 3);
                unsigned char mask = 1 << (7 - (px & 7));
                if (pix4 == 3) fb[idx] |= mask;
                else if (pix4 == 0) fb[idx] &= ~mask;
                else fb[idx] ^= mask;
            }
            
            // Draw pixel 5
            if (pix5 != 2) {
                unsigned char px = (X + 5) & 127;
                unsigned int idx = row_offset + (px >> 3);
                unsigned char mask = 1 << (7 - (px & 7));
                if (pix5 == 3) fb[idx] |= mask;
                else if (pix5 == 0) fb[idx] &= ~mask;
                else fb[idx] ^= mask;
            }
            
            // Draw pixel 6
            if (pix6 != 2) {
                unsigned char px = (X + 6) & 127;
                unsigned int idx = row_offset + (px >> 3);
                unsigned char mask = 1 << (7 - (px & 7));
                if (pix6 == 3) fb[idx] |= mask;
                else if (pix6 == 0) fb[idx] &= ~mask;
                else fb[idx] ^= mask;
            }
            
            // Draw pixel 7
            if (pix7 != 2) {
                unsigned char px = (X + 7) & 127;
                unsigned int idx = row_offset + (px >> 3);
                unsigned char mask = 1 << (7 - (px & 7));
                if (pix7 == 3) fb[idx] |= mask;
                else if (pix7 == 0) fb[idx] &= ~mask;
                else fb[idx] ^= mask;
            }
        }
    } else {
        // NON-WARP MODE - clipping
        for (unsigned char r = 0; r < 8; r++) {
            int py = Y + r;
            if (py < 0 || py >= 64) continue;
            
            unsigned int row_offset = (unsigned int)py << 4;
            unsigned char tr = tile_2bpp[r << 1];
            unsigned char tr2 = tile_2bpp[(r << 1) + 1];
            
            // Pre-extract all 8 pixels
            unsigned char pix0 = (tr >> 6) & 3;
            unsigned char pix1 = (tr >> 4) & 3;
            unsigned char pix2 = (tr >> 2) & 3;
            unsigned char pix3 = tr & 3;
            unsigned char pix4 = (tr2 >> 6) & 3;
            unsigned char pix5 = (tr2 >> 4) & 3;
            unsigned char pix6 = (tr2 >> 2) & 3;
            unsigned char pix7 = tr2 & 3;
            
            // Draw pixel 0
            int px0 = X + 0;
            if ((unsigned int)px0 < 128 && pix0 != 2) {
                unsigned int idx = row_offset + (px0 >> 3);
                unsigned char mask = 1 << (7 - (px0 & 7));
                if (pix0 == 3) fb[idx] |= mask;
                else if (pix0 == 0) fb[idx] &= ~mask;
                else fb[idx] ^= mask;
            }
            
            // Draw pixel 1
            int px1 = X + 1;
            if ((unsigned int)px1 < 128 && pix1 != 2) {
                unsigned int idx = row_offset + (px1 >> 3);
                unsigned char mask = 1 << (7 - (px1 & 7));
                if (pix1 == 3) fb[idx] |= mask;
                else if (pix1 == 0) fb[idx] &= ~mask;
                else fb[idx] ^= mask;
            }
            
            // Draw pixel 2
            int px2 = X + 2;
            if ((unsigned int)px2 < 128 && pix2 != 2) {
                unsigned int idx = row_offset + (px2 >> 3);
                unsigned char mask = 1 << (7 - (px2 & 7));
                if (pix2 == 3) fb[idx] |= mask;
                else if (pix2 == 0) fb[idx] &= ~mask;
                else fb[idx] ^= mask;
            }
            
            // Draw pixel 3
            int px3 = X + 3;
            if ((unsigned int)px3 < 128 && pix3 != 2) {
                unsigned int idx = row_offset + (px3 >> 3);
                unsigned char mask = 1 << (7 - (px3 & 7));
                if (pix3 == 3) fb[idx] |= mask;
                else if (pix3 == 0) fb[idx] &= ~mask;
                else fb[idx] ^= mask;
            }
            
            // Draw pixel 4
            int px4 = X + 4;
            if ((unsigned int)px4 < 128 && pix4 != 2) {
                unsigned int idx = row_offset + (px4 >> 3);
                unsigned char mask = 1 << (7 - (px4 & 7));
                if (pix4 == 3) fb[idx] |= mask;
                else if (pix4 == 0) fb[idx] &= ~mask;
                else fb[idx] ^= mask;
            }
            
            // Draw pixel 5
            int px5 = X + 5;
            if ((unsigned int)px5 < 128 && pix5 != 2) {
                unsigned int idx = row_offset + (px5 >> 3);
                unsigned char mask = 1 << (7 - (px5 & 7));
                if (pix5 == 3) fb[idx] |= mask;
                else if (pix5 == 0) fb[idx] &= ~mask;
                else fb[idx] ^= mask;
            }
            
            // Draw pixel 6
            int px6 = X + 6;
            if ((unsigned int)px6 < 128 && pix6 != 2) {
                unsigned int idx = row_offset + (px6 >> 3);
                unsigned char mask = 1 << (7 - (px6 & 7));
                if (pix6 == 3) fb[idx] |= mask;
                else if (pix6 == 0) fb[idx] &= ~mask;
                else fb[idx] ^= mask;
            }
            
            // Draw pixel 7
            int px7 = X + 7;
            if ((unsigned int)px7 < 128 && pix7 != 2) {
                unsigned int idx = row_offset + (px7 >> 3);
                unsigned char mask = 1 << (7 - (px7 & 7));
                if (pix7 == 3) fb[idx] |= mask;
                else if (pix7 == 0) fb[idx] &= ~mask;
                else fb[idx] ^= mask;
            }
        }
    }
}

static void fetch_tile_from_tilemap_2bpp(int N) {
    unsigned char base_Y = N >> 4;
    unsigned char base_X = N & 0x0F;
    
    unsigned int base_addr = (base_Y << 8) + (base_X << 1);
    const unsigned char *ptr = &tilemap_2bpp[base_addr];
    
    for (unsigned char i = 0; i < 8; i++) {
        tile_2bpp[i<<1] = pgm_read_byte(ptr);
        tile_2bpp[(i<<1) + 1] = pgm_read_byte(ptr + 1);
        ptr += 32;
    }
}

void draw_sprite_2bpp(SPRITE Sp, int X, int Y, unsigned char wrap){
    int base_y = Y;
    
    for (unsigned char row = 0; row < Sp.height_tile; row++) {
        int base_x = X;
        unsigned char row_offset = row * Sp.width_tile;
        
        for (unsigned char col = 0; col < Sp.width_tile; col++) {
            unsigned char tile_index = col + row_offset;
            
            // Read from PROGMEM
            unsigned char tile_num = pgm_read_byte(&Sp.sprite_tiles[tile_index]);
            unsigned char trans = pgm_read_byte(&Sp.trans_tiles[tile_index]);
            
            fetch_tile_from_tilemap_2bpp(tile_num);
            
            // Handle transformations
            if (trans & 0x01) SYM_H_2bpp();  // bit 0 = horizontal flip
            if (trans & 0x02) SYM_V_2bpp();  // bit 1 = vertical flip
            
            draw_tile_2bpp(base_x, base_y, wrap);
            
            base_x += 8;
        }
        base_y += 8;
    }
}

static void fetch_tile_from_tilemap_1bpp(int N) {
    unsigned char base_Y = N >> 4;  // N/16 -> bit shift
    unsigned char base_X = N & 0x0F; // N%16 -> bit mask
    unsigned int base_addr = (base_Y << 7) + base_X; // base_Y*128 + base_X
    
    for (unsigned char i = 0; i < 8; i++) {
        tile_1bpp[i] = pgm_read_byte(&tilemap_1bpp[base_addr]);
        base_addr += 16;
    }
}

// void draw_tile_1bpp(int X, int Y) {
//     unsigned char *fb = current_buffer;
//     for (unsigned char r = 0; r < 8; r++) {
//         int py = Y + r;
//         if (py < 0 || py >= 64) continue; // clip vertical bounds

//         unsigned int row_offset = py << 4; // 16 bytes per row (128 pixels width)
//         unsigned char row = tile_1bpp[r];       // 1 byte per tile row

//         int x_byte = X >> 3;
//         int x_bit = X & 7;

//         if (x_byte < -1 || x_byte >= 16) continue; // completely outside horizontal bounds

//         if (x_bit == 0) {
//             // perfectly aligned: write full byte
//             if (x_byte >= 0 && x_byte < 16) {
//                 fb[row_offset + x_byte] = row;
//             }
//         } else {
//             // misaligned: split into left and right bytes
//             unsigned char left = row >> x_bit;
//             unsigned char right = row << (8 - x_bit);

//             // left byte
//             if (x_byte >= 0 && x_byte < 16) {
//                 fb[row_offset + x_byte] &= ~(0xFF >> x_bit);
//                 fb[row_offset + x_byte] |= left;
//             }

//             // right byte
//             int next_byte = x_byte + 1;
//             if (next_byte >= 0 && next_byte < 16) {
//                 fb[row_offset + next_byte] &= (0xFF >> (8 - x_bit));
//                 fb[row_offset + next_byte] |= right;
//             }
//         }
//     }
// }

static inline void draw_tile_1bpp(int X, int Y) {
    unsigned char *fb = current_buffer;
    
    int x_byte = X >> 3;
    int x_bit = X & 7;
    
    // Early exit for completely out of bounds
    if (x_byte < -1 || x_byte >= 16) return;
    
    // Aligned fast path - most common case
    if (x_bit == 0 && x_byte >= 0 && x_byte < 16) {
        for (unsigned char r = 0; r < 8; r++) {
            int py = Y + r;
            if (py >= 0 && py < 64) {
                fb[(py << 4) + x_byte] = tile_1bpp[r];
            }
        }
        return;
    }
    
    // Misaligned path
    unsigned char left_mask = 0xFF << (8 - x_bit);
    unsigned char right_mask = 0xFF >> x_bit;
    
    for (unsigned char r = 0; r < 8; r++) {
        int py = Y + r;
        if (py < 0 || py >= 64) continue;
        
        unsigned int row_offset = py << 4;
        unsigned char row = tile_1bpp[r];
        
        unsigned char left = row >> x_bit;
        unsigned char right = row << (8 - x_bit);
        
        // Left byte
        if (x_byte >= 0 && x_byte < 16) {
            fb[row_offset + x_byte] = (fb[row_offset + x_byte] & left_mask) | left;
        }
        
        // Right byte
        int next_byte = x_byte + 1;
        if (next_byte >= 0 && next_byte < 16) {
            fb[row_offset + next_byte] = (fb[row_offset + next_byte] & right_mask) | right;
        }
    }
}

void draw_sprite_1bpp(SPRITE Sp, int X, int Y){
    for (unsigned char row = 0; row < Sp.height_tile; row++) {
        for (unsigned char col = 0; col < Sp.width_tile; col++) {
            unsigned char tile_index = col + row * Sp.width_tile;
            
            // Read from PROGMEM!
            unsigned char tile_num = pgm_read_byte(&Sp.sprite_tiles[tile_index]);
            unsigned char trans = pgm_read_byte(&Sp.trans_tiles[tile_index]);
            
            fetch_tile_from_tilemap_1bpp(tile_num);

            switch (trans) {
                case 0: // normal draw
                    draw_tile_1bpp(X + (col << 3) , Y + (row << 3));
                    break;

                case 1:
                    SYM_H_1bpp();
                    draw_tile_1bpp(X + (col << 3), Y + (row << 3));
                    break;

                case 2:
                    SYM_V_1bpp();
                    draw_tile_1bpp(X + (col << 3), Y + row << 3);
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
