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
// #define SAFE_SHIFT(table, shift)  ((shift) >= 0 ? (table) << (shift) : (table) >> ((-(shift)) & 7))

inline void SYM_H_2bpp(void)
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


inline void SYM_V_2bpp(void)
{
    register unsigned char tmp0,tmp1;

    tmp0 = tile_2bpp[0]; tmp1 = tile_2bpp[1]; tile_2bpp[0] = tile_2bpp[14]; tile_2bpp[1] = tile_2bpp[15]; tile_2bpp[14] = tmp0; tile_2bpp[15] = tmp1;
    tmp0 = tile_2bpp[2]; tmp1 = tile_2bpp[3]; tile_2bpp[2] = tile_2bpp[12]; tile_2bpp[3] = tile_2bpp[13]; tile_2bpp[12] = tmp0; tile_2bpp[13] = tmp1;
    tmp0 = tile_2bpp[4]; tmp1 = tile_2bpp[5]; tile_2bpp[4] = tile_2bpp[10]; tile_2bpp[5] = tile_2bpp[11]; tile_2bpp[10] = tmp0; tile_2bpp[11] = tmp1;
    tmp0 = tile_2bpp[6]; tmp1 = tile_2bpp[7]; tile_2bpp[6] = tile_2bpp[8];  tile_2bpp[7] = tile_2bpp[9];  tile_2bpp[8]  = tmp0; tile_2bpp[9]  = tmp1;
}

inline void ROT_90CW_2bpp(void)
{
    unsigned char tmp[16];
    
    tmp[0]  = ((tile_2bpp[1] & 0x03) << 6) | ((tile_2bpp[3] & 0x03) << 4) | ((tile_2bpp[5] & 0x03) << 2) | (tile_2bpp[7] & 0x03);
    tmp[2]  = ((tile_2bpp[1] & 0x0C) << 4) | ((tile_2bpp[3] & 0x0C) << 2) | (tile_2bpp[5] & 0x0C) | ((tile_2bpp[7] & 0x0C) >> 2);
    tmp[4]  = ((tile_2bpp[1] & 0x30) << 2) | (tile_2bpp[3] & 0x30) | ((tile_2bpp[5] & 0x30) >> 2) | ((tile_2bpp[7] & 0x30) >> 4);
    tmp[6]  = (tile_2bpp[1] & 0xC0) | ((tile_2bpp[3] & 0xC0) >> 2) | ((tile_2bpp[5] & 0xC0) >> 4) | ((tile_2bpp[7] & 0xC0) >> 6);
    tmp[8]  = ((tile_2bpp[0] & 0x03) << 6) | ((tile_2bpp[2] & 0x03) << 4) | ((tile_2bpp[4] & 0x03) << 2) | (tile_2bpp[6] & 0x03);
    tmp[10] = ((tile_2bpp[0] & 0x0C) << 4) | ((tile_2bpp[2] & 0x0C) << 2) | (tile_2bpp[4] & 0x0C) | ((tile_2bpp[6] & 0x0C) >> 2);
    tmp[12] = ((tile_2bpp[0] & 0x30) << 2) | (tile_2bpp[2] & 0x30) | ((tile_2bpp[4] & 0x30) >> 2) | ((tile_2bpp[6] & 0x30) >> 4);
    tmp[14] = (tile_2bpp[0] & 0xC0) | ((tile_2bpp[2] & 0xC0) >> 2) | ((tile_2bpp[4] & 0xC0) >> 4) | ((tile_2bpp[6] & 0xC0) >> 6);
    
    
    tmp[1]  = ((tile_2bpp[9] & 0x03) << 6) | ((tile_2bpp[11] & 0x03) << 4) | ((tile_2bpp[13] & 0x03) << 2) | (tile_2bpp[15] & 0x03);
    tmp[3]  = ((tile_2bpp[9] & 0x0C) << 4) | ((tile_2bpp[11] & 0x0C) << 2) | (tile_2bpp[13] & 0x0C) | ((tile_2bpp[15] & 0x0C) >> 2);
    tmp[5]  = ((tile_2bpp[9] & 0x30) << 2) | (tile_2bpp[11] & 0x30) | ((tile_2bpp[13] & 0x30) >> 2) | ((tile_2bpp[15] & 0x30) >> 4);
    tmp[7]  = (tile_2bpp[9] & 0xC0) | ((tile_2bpp[11] & 0xC0) >> 2) | ((tile_2bpp[13] & 0xC0) >> 4) | ((tile_2bpp[15] & 0xC0) >> 6);
    tmp[9]  = ((tile_2bpp[8] & 0x03) << 6) | ((tile_2bpp[10] & 0x03) << 4) | ((tile_2bpp[12] & 0x03) << 2) | (tile_2bpp[14] & 0x03);
    tmp[11] = ((tile_2bpp[8] & 0x0C) << 4) | ((tile_2bpp[10] & 0x0C) << 2) | (tile_2bpp[12] & 0x0C) | ((tile_2bpp[14] & 0x0C) >> 2);
    tmp[13] = ((tile_2bpp[8] & 0x30) << 2) | (tile_2bpp[10] & 0x30) | ((tile_2bpp[12] & 0x30) >> 2) | ((tile_2bpp[14] & 0x30) >> 4);
    tmp[15] = (tile_2bpp[8] & 0xC0) | ((tile_2bpp[10] & 0xC0) >> 2) | ((tile_2bpp[12] & 0xC0) >> 4) | ((tile_2bpp[14] & 0xC0) >> 6);

    // Copy back 
    tile_2bpp[0]  = tmp[0];
    tile_2bpp[1]  = tmp[1];
    tile_2bpp[2]  = tmp[2];
    tile_2bpp[3]  = tmp[3];
    tile_2bpp[4]  = tmp[4];
    tile_2bpp[5]  = tmp[5];
    tile_2bpp[6]  = tmp[6];
    tile_2bpp[7]  = tmp[7];
    tile_2bpp[8]  = tmp[8];
    tile_2bpp[9]  = tmp[9];
    tile_2bpp[10] = tmp[10];
    tile_2bpp[11] = tmp[11];
    tile_2bpp[12] = tmp[12];
    tile_2bpp[13] = tmp[13];
    tile_2bpp[14] = tmp[14];
    tile_2bpp[15] = tmp[15];
}


inline void ROT_90CCW_2bpp(void)
{
    unsigned char tmp[16];

    tmp[1]  = ((tile_2bpp[0] & 0xC0) >> 6) | ((tile_2bpp[2] & 0xC0) >> 4) | ((tile_2bpp[4] & 0xC0) >> 2) | (tile_2bpp[6] & 0xC0);
    tmp[3]  = ((tile_2bpp[0] & 0x30) >> 4) | ((tile_2bpp[2] & 0x30) >> 2) | (tile_2bpp[4] & 0x30) | ((tile_2bpp[6] & 0x30) << 2);
    tmp[5]  = ((tile_2bpp[0] & 0x0C) >> 2) | (tile_2bpp[2] & 0x0C) | ((tile_2bpp[4] & 0x0C) << 2) | ((tile_2bpp[6] & 0x0C) << 4);
    tmp[7]  = (tile_2bpp[0] & 0x03) | ((tile_2bpp[2] & 0x03) << 2) | ((tile_2bpp[4] & 0x03) << 4) | ((tile_2bpp[6] & 0x03) << 6);
    tmp[9]  = ((tile_2bpp[1] & 0xC0) >> 6) | ((tile_2bpp[3] & 0xC0) >> 4) | ((tile_2bpp[5] & 0xC0) >> 2) | (tile_2bpp[7] & 0xC0);
    tmp[11] = ((tile_2bpp[1] & 0x30) >> 4) | ((tile_2bpp[3] & 0x30) >> 2) | (tile_2bpp[5] & 0x30) | ((tile_2bpp[7] & 0x30) << 2);
    tmp[13] = ((tile_2bpp[1] & 0x0C) >> 2) | (tile_2bpp[3] & 0x0C) | ((tile_2bpp[5] & 0x0C) << 2) | ((tile_2bpp[7] & 0x0C) << 4);
    tmp[15] = (tile_2bpp[1] & 0x03) | ((tile_2bpp[3] & 0x03) << 2) | ((tile_2bpp[5] & 0x03) << 4) | ((tile_2bpp[7] & 0x03) << 6);
    
    
    tmp[0]  = ((tile_2bpp[8] & 0xC0) >> 6) | ((tile_2bpp[10] & 0xC0) >> 4) | ((tile_2bpp[12] & 0xC0) >> 2) | (tile_2bpp[14] & 0xC0);
    tmp[2]  = ((tile_2bpp[8] & 0x30) >> 4) | ((tile_2bpp[10] & 0x30) >> 2) | (tile_2bpp[12] & 0x30) | ((tile_2bpp[14] & 0x30) << 2);
    tmp[4]  = ((tile_2bpp[8] & 0x0C) >> 2) | (tile_2bpp[10] & 0x0C) | ((tile_2bpp[12] & 0x0C) << 2) | ((tile_2bpp[14] & 0x0C) << 4);
    tmp[6]  = (tile_2bpp[8] & 0x03) | ((tile_2bpp[10] & 0x03) << 2) | ((tile_2bpp[12] & 0x03) << 4) | ((tile_2bpp[14] & 0x03) << 6);
    tmp[8]  = ((tile_2bpp[9] & 0xC0) >> 6) | ((tile_2bpp[11] & 0xC0) >> 4) | ((tile_2bpp[13] & 0xC0) >> 2) | (tile_2bpp[15] & 0xC0);
    tmp[10] = ((tile_2bpp[9] & 0x30) >> 4) | ((tile_2bpp[11] & 0x30) >> 2) | (tile_2bpp[13] & 0x30) | ((tile_2bpp[15] & 0x30) << 2);
    tmp[12] = ((tile_2bpp[9] & 0x0C) >> 2) | (tile_2bpp[11] & 0x0C) | ((tile_2bpp[13] & 0x0C) << 2) | ((tile_2bpp[15] & 0x0C) << 4);
    tmp[14] = (tile_2bpp[9] & 0x03) | ((tile_2bpp[11] & 0x03) << 2) | ((tile_2bpp[13] & 0x03) << 4) | ((tile_2bpp[15] & 0x03) << 6);

    // Copy back
    tile_2bpp[0]  = tmp[0];
    tile_2bpp[1]  = tmp[1];
    tile_2bpp[2]  = tmp[2];
    tile_2bpp[3]  = tmp[3];
    tile_2bpp[4]  = tmp[4];
    tile_2bpp[5]  = tmp[5];
    tile_2bpp[6]  = tmp[6];
    tile_2bpp[7]  = tmp[7];
    tile_2bpp[8]  = tmp[8];
    tile_2bpp[9]  = tmp[9];
    tile_2bpp[10] = tmp[10];
    tile_2bpp[11] = tmp[11];
    tile_2bpp[12] = tmp[12];
    tile_2bpp[13] = tmp[13];
    tile_2bpp[14] = tmp[14];
    tile_2bpp[15] = tmp[15];
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

    tmp = tile_1bpp[0]; tile_1bpp[0] = tile_1bpp[7]; tile_1bpp[7] = tmp;
    tmp = tile_1bpp[1]; tile_1bpp[1] = tile_1bpp[6]; tile_1bpp[6] = tmp;
    tmp = tile_1bpp[2]; tile_1bpp[2] = tile_1bpp[5]; tile_1bpp[5] = tmp;
    tmp = tile_1bpp[3]; tile_1bpp[3] = tile_1bpp[4]; tile_1bpp[4] = tmp;
}

inline void ROT_90CW_1bpp(void)
{
    unsigned char tmp[8];

    tmp[0] = (tile_1bpp[7] & 0x01) << 7 | (tile_1bpp[6] & 0x01) << 6 | (tile_1bpp[5] & 0x01) << 5 | (tile_1bpp[4] & 0x01) << 4 | (tile_1bpp[3] & 0x01) << 3 | (tile_1bpp[2] & 0x01) << 2 | (tile_1bpp[1] & 0x01) << 1 | (tile_1bpp[0] & 0x01) << 0;
    tmp[1] = (tile_1bpp[7] & 0x02) << 6 | (tile_1bpp[6] & 0x02) << 5 | (tile_1bpp[5] & 0x02) << 4 | (tile_1bpp[4] & 0x02) << 3 | (tile_1bpp[3] & 0x02) << 2 | (tile_1bpp[2] & 0x02) << 1 | (tile_1bpp[1] & 0x02) << 0 | (tile_1bpp[0] & 0x02) >> 1;
    tmp[2] = (tile_1bpp[7] & 0x04) << 5 | (tile_1bpp[6] & 0x04) << 4 | (tile_1bpp[5] & 0x04) << 3 | (tile_1bpp[4] & 0x04) << 2 | (tile_1bpp[3] & 0x04) << 1 | (tile_1bpp[2] & 0x04) << 0 | (tile_1bpp[1] & 0x04) >> 1 | (tile_1bpp[0] & 0x04) >> 2;
    tmp[3] = (tile_1bpp[7] & 0x08) << 4 | (tile_1bpp[6] & 0x08) << 3 | (tile_1bpp[5] & 0x08) << 2 | (tile_1bpp[4] & 0x08) << 1 | (tile_1bpp[3] & 0x08) << 0 | (tile_1bpp[2] & 0x08) >> 1 | (tile_1bpp[1] & 0x08) >> 2 | (tile_1bpp[0] & 0x08) >> 3;
    tmp[4] = (tile_1bpp[7] & 0x10) << 3 | (tile_1bpp[6] & 0x10) << 2 | (tile_1bpp[5] & 0x10) << 1 | (tile_1bpp[4] & 0x10) << 0 | (tile_1bpp[3] & 0x10) >> 1 | (tile_1bpp[2] & 0x10) >> 2 | (tile_1bpp[1] & 0x10) >> 3 | (tile_1bpp[0] & 0x10) >> 4;
    tmp[5] = (tile_1bpp[7] & 0x20) << 2 | (tile_1bpp[6] & 0x20) << 1 | (tile_1bpp[5] & 0x20) << 0 | (tile_1bpp[4] & 0x20) >> 1 | (tile_1bpp[3] & 0x20) >> 2 | (tile_1bpp[2] & 0x20) >> 3 | (tile_1bpp[1] & 0x20) >> 4 | (tile_1bpp[0] & 0x20) >> 5;
    tmp[6] = (tile_1bpp[7] & 0x40) << 1 | (tile_1bpp[6] & 0x40) << 0 | (tile_1bpp[5] & 0x40) >> 1 | (tile_1bpp[4] & 0x40) >> 2 | (tile_1bpp[3] & 0x40) >> 3 | (tile_1bpp[2] & 0x40) >> 4 | (tile_1bpp[1] & 0x40) >> 5 | (tile_1bpp[0] & 0x40) >> 6;
    tmp[7] = (tile_1bpp[7] & 0x80) << 0 | (tile_1bpp[6] & 0x80) >> 1 | (tile_1bpp[5] & 0x80) >> 2 | (tile_1bpp[4] & 0x80) >> 3 | (tile_1bpp[3] & 0x80) >> 4 | (tile_1bpp[2] & 0x80) >> 5 | (tile_1bpp[1] & 0x80) >> 6 | (tile_1bpp[0] & 0x80) >> 7;

    // Copy back
    tile_1bpp[0] = tmp[0];
    tile_1bpp[1] = tmp[1];
    tile_1bpp[2] = tmp[2];
    tile_1bpp[3] = tmp[3];
    tile_1bpp[4] = tmp[4];
    tile_1bpp[5] = tmp[5];
    tile_1bpp[6] = tmp[6];
    tile_1bpp[7] = tmp[7];
}


inline void ROT_90CCW_1bpp(void)
{
    unsigned char tmp[8];
    
    tmp[0] = (tile_1bpp[0] & 0x01) << 7 | (tile_1bpp[1] & 0x01) << 6 | (tile_1bpp[2] & 0x01) << 5 | (tile_1bpp[3] & 0x01) << 4 | (tile_1bpp[4] & 0x01) << 3 | (tile_1bpp[5] & 0x01) << 2 | (tile_1bpp[6] & 0x01) << 1 | (tile_1bpp[7] & 0x01) << 0;
    tmp[1] = (tile_1bpp[0] & 0x02) << 6 | (tile_1bpp[1] & 0x02) << 5 | (tile_1bpp[2] & 0x02) << 4 | (tile_1bpp[3] & 0x02) << 3 | (tile_1bpp[4] & 0x02) << 2 | (tile_1bpp[5] & 0x02) << 1 | (tile_1bpp[6] & 0x02) << 0 | (tile_1bpp[7] & 0x02) >> 1;
    tmp[2] = (tile_1bpp[0] & 0x04) << 5 | (tile_1bpp[1] & 0x04) << 4 | (tile_1bpp[2] & 0x04) << 3 | (tile_1bpp[3] & 0x04) << 2 | (tile_1bpp[4] & 0x04) << 1 | (tile_1bpp[5] & 0x04) << 0 | (tile_1bpp[6] & 0x04) >> 1 | (tile_1bpp[7] & 0x04) >> 2;
    tmp[3] = (tile_1bpp[0] & 0x08) << 4 | (tile_1bpp[1] & 0x08) << 3 | (tile_1bpp[2] & 0x08) << 2 | (tile_1bpp[3] & 0x08) << 1 | (tile_1bpp[4] & 0x08) << 0 | (tile_1bpp[5] & 0x08) >> 1 | (tile_1bpp[6] & 0x08) >> 2 | (tile_1bpp[7] & 0x08) >> 3;
    tmp[4] = (tile_1bpp[0] & 0x10) << 3 | (tile_1bpp[1] & 0x10) << 2 | (tile_1bpp[2] & 0x10) << 1 | (tile_1bpp[3] & 0x10) << 0 | (tile_1bpp[4] & 0x10) >> 1 | (tile_1bpp[5] & 0x10) >> 2 | (tile_1bpp[6] & 0x10) >> 3 | (tile_1bpp[7] & 0x10) >> 4;
    tmp[5] = (tile_1bpp[0] & 0x20) << 2 | (tile_1bpp[1] & 0x20) << 1 | (tile_1bpp[2] & 0x20) << 0 | (tile_1bpp[3] & 0x20) >> 1 | (tile_1bpp[4] & 0x20) >> 2 | (tile_1bpp[5] & 0x20) >> 3 | (tile_1bpp[6] & 0x20) >> 4 | (tile_1bpp[7] & 0x20) >> 5;
    tmp[6] = (tile_1bpp[0] & 0x40) << 1 | (tile_1bpp[1] & 0x40) << 0 | (tile_1bpp[2] & 0x40) >> 1 | (tile_1bpp[3] & 0x40) >> 2 | (tile_1bpp[4] & 0x40) >> 3 | (tile_1bpp[5] & 0x40) >> 4 | (tile_1bpp[6] & 0x40) >> 5 | (tile_1bpp[7] & 0x40) >> 6;
    tmp[7] = (tile_1bpp[0] & 0x80) << 0 | (tile_1bpp[1] & 0x80) >> 1 | (tile_1bpp[2] & 0x80) >> 2 | (tile_1bpp[3] & 0x80) >> 3 | (tile_1bpp[4] & 0x80) >> 4 | (tile_1bpp[5] & 0x80) >> 5 | (tile_1bpp[6] & 0x80) >> 6 | (tile_1bpp[7] & 0x80) >> 7;

    // Copy back
    tile_1bpp[0] = tmp[0];
    tile_1bpp[1] = tmp[1];
    tile_1bpp[2] = tmp[2];
    tile_1bpp[3] = tmp[3];
    tile_1bpp[4] = tmp[4];
    tile_1bpp[5] = tmp[5];
    tile_1bpp[6] = tmp[6];
    tile_1bpp[7] = tmp[7];
}

void draw_tile_2bpp(int X, int Y) {
    unsigned char *fb = current_buffer;
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

inline void fetch_tile_from_tilemap_2bpp(int N) {
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

void draw_sprite_2bpp(SPRITE Sp, int X, int Y){
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
            
            draw_tile_2bpp(base_x, base_y);
            
            base_x += 8;
        }
        base_y += 8;
    }
}

inline void fetch_tile_from_tilemap_1bpp(int N) {
    unsigned int base_addr = ((N >> 4) << 7) + (N & 0x0F);
    const unsigned char *src = &tilemap_1bpp[base_addr];
    
    tile_1bpp[0] = pgm_read_byte(src);      src += 16;
    tile_1bpp[1] = pgm_read_byte(src);      src += 16;
    tile_1bpp[2] = pgm_read_byte(src);      src += 16;
    tile_1bpp[3] = pgm_read_byte(src);      src += 16;
    tile_1bpp[4] = pgm_read_byte(src);      src += 16;
    tile_1bpp[5] = pgm_read_byte(src);      src += 16;
    tile_1bpp[6] = pgm_read_byte(src);      src += 16;
    tile_1bpp[7] = pgm_read_byte(src);
}

void draw_tile_1bpp(int X, int Y) {
    int x_byte = X >> 3;
    if (x_byte < -1 || x_byte >= 16) return;
    
    unsigned char *fb = current_buffer;
    int x_bit = X & 7;
    
    // Aligned fast path
    if (x_bit == 0 && x_byte >= 0 && x_byte < 16 && Y >= 0 && Y < 64) {
        unsigned int idx = (Y << 4) + x_byte;
        fb[idx]      = tile_1bpp[0];
        fb[idx + 16] = tile_1bpp[1];
        fb[idx + 32] = tile_1bpp[2];
        fb[idx + 48] = tile_1bpp[3];
        fb[idx + 64] = tile_1bpp[4];
        fb[idx + 80] = tile_1bpp[5];
        fb[idx + 96] = tile_1bpp[6];
        fb[idx + 112] = tile_1bpp[7];
        return;
    }
    
    // Misaligned path
    unsigned char lm = 0xFF << (8 - x_bit);
    unsigned char rm = 0xFF >> x_bit;
    unsigned char sr = x_bit;
    unsigned char sl = 8 - x_bit;
    
    for (unsigned char r = 0; r < 8; r++) {
        int py = Y + r;
        if (py < 0 || py >= 64) continue;
        
        unsigned int offs = py << 4;
        unsigned char row = tile_1bpp[r];
        unsigned char left = row >> sr;
        unsigned char right = row << sl;
        
        if (x_byte >= 0 && x_byte < 16) {
            unsigned int idx = offs + x_byte;
            fb[idx] = (fb[idx] & lm) | left;
        }
        if (x_byte < 15) {
            unsigned int idx = offs + x_byte + 1;
            fb[idx] = (fb[idx] & rm) | right;
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

void draw_tilemap_1bpp(void)
{
    for (unsigned char row= 0; row < 8; row++) 
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
    for (unsigned char row= 0; row < 4; row++) 
    {
        for (unsigned char col = 0; col < 16; col++) 
        {
            unsigned char tile_index = col + row * 16;
            fetch_tile_from_tilemap_2bpp(tile_index);
            draw_tile_2bpp(col << 3, row << 3);
        }
    }
}


void draw_rectangle(int X, int Y, unsigned char width_in_tiles, unsigned char height_in_tiles) {
    // Clip to visible screen area
    if (X >= 128 || Y >= 64) return;
    int right_calc = X + (width_in_tiles << 3);
    int bottom_calc = Y + (height_in_tiles << 3);
    // If completely off-screen on left or top
    if (right_calc <= 0 || bottom_calc <= 0) return;
    // Clip to screen bounds
    unsigned char start_x = (X < 0) ? 0 : X;
    unsigned char start_y = (Y < 0) ? 0 : Y;
    unsigned char end_x = (right_calc > 128) ? 127 : right_calc - 1;
    unsigned char end_y = (bottom_calc > 64) ? 63 : bottom_calc - 1;
    unsigned char draw_top = (Y >= 0);
    unsigned char draw_bottom = (bottom_calc <= 64);
    unsigned char draw_left = (X >= 0);
    unsigned char draw_right = (right_calc <= 128);
    unsigned char x_byte = start_x >> 3;
    unsigned char end_x_byte = end_x >> 3;
    unsigned char x_bit = start_x & 7;
    unsigned char end_x_bit = end_x & 7;
    unsigned char width_bytes = end_x_byte - x_byte;
    unsigned char left_mask = 0xFF >> x_bit;
    unsigned char right_mask = 0xFF << (7 - end_x_bit);
    unsigned char left_pix = 0x80 >> x_bit;
    unsigned char right_pix = 0x80 >> end_x_bit;
    
    // Top row
    if (draw_top) {
        unsigned int idx = (start_y << 4) + x_byte;
        if (width_bytes) {
            if (draw_left) current_buffer[idx] |= left_mask;
            else current_buffer[idx] = 0xFF; // Fill if left is clipped
            idx++;
            unsigned char b = width_bytes - 1;
            while (b--) current_buffer[idx++] = 0xFF;
            // Always draw the right edge if we have width_bytes > 0
            if (draw_right) current_buffer[idx] |= right_mask;
            else current_buffer[idx] = 0xFF; // Fill to edge if right is clipped
        } else {
            unsigned char mask = left_mask & right_mask;
            if (draw_left && draw_right) current_buffer[idx] |= mask;
            else if (draw_left) current_buffer[idx] |= left_mask;
            else if (draw_right) current_buffer[idx] |= right_mask;
            else current_buffer[idx] = 0xFF; // Both clipped
        }
    }
    
    // Middle rows - only borders
    if (end_y > start_y + 1) {
        unsigned char rows = end_y - start_y - 1;
        unsigned int idx = ((start_y + 1) << 4) + x_byte;
        if (width_bytes) {
            do {
                if (draw_left) current_buffer[idx] |= left_pix;
                if (draw_right) current_buffer[idx + width_bytes] |= right_pix;
                idx += 16;
            } while (--rows);
        } else {
            unsigned char border = 0;
            if (draw_left) border |= left_pix;
            if (draw_right) border |= right_pix;
            if (border) {
                do {
                    current_buffer[idx] |= border;
                    idx += 16;
                } while (--rows);
            }
        }
    }
    
    // Bottom row
    if (draw_bottom && end_y != start_y) {
        unsigned int idx = (end_y << 4) + x_byte;
        if (width_bytes) {
            if (draw_left) current_buffer[idx] |= left_mask;
            else current_buffer[idx] = 0xFF;
            idx++;
            unsigned char b = width_bytes - 1;
            while (b--) current_buffer[idx++] = 0xFF;
            if (draw_right) current_buffer[idx] |= right_mask;
            else current_buffer[idx] = 0xFF;
        } else {
            unsigned char mask = left_mask & right_mask;
            if (draw_left && draw_right) current_buffer[idx] |= mask;
            else if (draw_left) current_buffer[idx] |= left_mask;
            else if (draw_right) current_buffer[idx] |= right_mask;
            else current_buffer[idx] = 0xFF;
        }
    }
}


void draw_line(int x0, int y0, int x1, int y1) {
    // Quick bounds check - if both points clearly outside, skip clipping
    if ((x0 < 0 && x1 < 0) || (x0 >= 128 && x1 >= 128) || 
        (y0 < 0 && y1 < 0) || (y0 >= 64 && y1 >= 64)) return;
    
    // Cohen-Sutherland line clipping (optimized)
    int out0 = ((x0 < 0) << 0) | ((x0 >= 128) << 1) | ((y0 < 0) << 2) | ((y0 >= 64) << 3);
    int out1 = ((x1 < 0) << 0) | ((x1 >= 128) << 1) | ((y1 < 0) << 2) | ((y1 >= 64) << 3);
    
    while (out0 | out1) {
        if (out0 & out1) return;
        
        int out = out0 ? out0 : out1;
        int x, y;
        
        if (out & 8) { // Above
            x = x0 + (x1 - x0) * (63 - y0) / (y1 - y0);
            y = 63;
        } else if (out & 4) { // Below
            x = x0 + (x1 - x0) * (-y0) / (y1 - y0);
            y = 0;
        } else if (out & 2) { // Right
            y = y0 + (y1 - y0) * (127 - x0) / (x1 - x0);
            x = 127;
        } else { // Left
            y = y0 + (y1 - y0) * (-x0) / (x1 - x0);
            x = 0;
        }
        
        if (out == out0) {
            x0 = x; y0 = y;
            out0 = ((x0 < 0) << 0) | ((x0 >= 128) << 1) | ((y0 < 0) << 2) | ((y0 >= 64) << 3);
        } else {
            x1 = x; y1 = y;
            out1 = ((x1 < 0) << 0) | ((x1 >= 128) << 1) | ((y1 < 0) << 2) | ((y1 >= 64) << 3);
        }
    }
    
    int dx = x1 - x0;
    int dy = y1 - y0;
    
    // Horizontal line (ultra-optimized)
    if (dy == 0) {
        if (dx < 0) { int t = x0; x0 = x1; x1 = t; }
        
        unsigned char start_byte = x0 >> 3;
        unsigned char end_byte = x1 >> 3;
        unsigned char start_bit = x0 & 7;
        unsigned char end_bit = x1 & 7;
        unsigned int idx = (y0 << 4) + start_byte;
        
        if (start_byte == end_byte) {
            // Single byte
            unsigned char mask = (0xFF >> start_bit) & (0xFF << (7 - end_bit));
            current_buffer[idx] |= mask;
        } else {
            // Multiple bytes
            current_buffer[idx++] |= 0xFF >> start_bit;
            unsigned char bytes = end_byte - start_byte - 1;
            while (bytes--) current_buffer[idx++] = 0xFF;
            current_buffer[idx] |= 0xFF << (7 - end_bit);
        }
        return;
    }
    
    // Vertical line (ultra-optimized)
    if (dx == 0) {
        if (dy < 0) { int t = y0; y0 = y1; y1 = t; }
        
        unsigned char x_byte = x0 >> 3;
        unsigned char x_mask = 0x80 >> (x0 & 7);
        unsigned int idx = (y0 << 4) + x_byte;
        unsigned char rows = y1 - y0 + 1;
        
        do {
            current_buffer[idx] |= x_mask;
            idx += 16;
        } while (--rows);
        return;
    }
    
    // Diagonal/general line (optimized Bresenham)
    int sx = (dx > 0) ? 1 : -1;
    int sy = (dy > 0) ? 1 : -1;
    
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    
    int err = dx - dy;
    int x = x0;
    int y = y0;
    
    // Precompute for faster inner loop
    int dx2 = dx << 1;
    int dy2 = dy << 1;
    
    // Unrolled for primary axis
    if (dx > dy) {
        // X-dominant
        do {
            unsigned int idx = (y << 4) + (x >> 3);
            current_buffer[idx] |= 0x80 >> (x & 7);
            
            if (x == x1) break;
            
            int e2 = err << 1;
            if (e2 > -dy2) { err -= dy; x += sx; }
            if (e2 < dx2) { err += dx; y += sy; }
        } while (1);
    } else {
        // Y-dominant
        do {
            unsigned int idx = (y << 4) + (x >> 3);
            current_buffer[idx] |= 0x80 >> (x & 7);
            
            if (y == y1) break;
            
            int e2 = err << 1;
            if (e2 > -dy2) { err -= dy; x += sx; }
            if (e2 < dx2) { err += dx; y += sy; }
        } while (1);
    }
}

// ! Worst implementation ever 
// TODO: Need to fix later 
void draw_char(char c, int X, int Y) {
    
    if (c < 32 || c > 126) return; // Unsupported character
    if (c > 96 && c <= 122){
        c -= 32;
    } 
    if (c > 122){
        c -= 26;
    } 
    
    fetch_tile_from_tilemap_1bpp(c+27);
    draw_tile_1bpp(X,Y);
}

void draw_string(const char* str, int X, int Y) {
    int x = X;
    int y = Y;
    unsigned char c_pos;
    
    while (*str) {
        if (*str == '\n') {
            x = X; // Reset to initial X
            y += 5; // Move down one character height
        } else {
            c_pos = *str;
            if (*str > 96) c_pos -= 32;
            draw_char(*str, x, y);
            x += pgm_read_byte(&font_spacing[c_pos-32]); // Move right character width
        }
        str++;
    }
}
