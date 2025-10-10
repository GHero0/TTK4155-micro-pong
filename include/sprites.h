#ifndef SPRITES_H
#define SPRITES_H

// #define ORI 0b00000000
// #define HFL 0b00000001
// #define VFL 0b00000010
// #define REV 0b00000011
// #define ROT 0b00000101
// #define SPN 0b00000110
// #define MDL 0b00001001
// #define ADL 0b00001010

#include "images.h"

void draw_sprite_1bpp(SPRITE Sp, int X, int Y);
void draw_sprite_2bpp(SPRITE Sp, int X, int Y);
void fetch_tile_from_tilemap_1bpp(int N);
void fetch_tile_from_tilemap_2bpp(int N);
void draw_tile_1bpp(int X, int Y);
void draw_tile_2bpp(int X, int Y);
void SYM_H_1bpp(void);
void SYM_V_1bpp(void);
void ROT_90CW_1bpp(void);
void ROT_90CCW_1bpp(void);
void SYM_H_2bpp(void);
void SYM_V_2bpp(void);
void ROT_90CW_2bpp(void);
void ROT_90CCW_2bpp(void);
void draw_tilemap_1bpp(void);
void draw_tilemap_2bpp(void);
void draw_rectangle(int X, int Y, unsigned char width_in_tiles, unsigned char height_in_tiles);
void draw_line(int x0, int y0, int x1, int y1);
void draw_char(char c, int X, int Y);
void draw_string(const char* str, int X, int Y);



#endif