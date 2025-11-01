#ifndef SPRITES_H
#define SPRITES_H

#include "images.h"

void draw_sprite_1bpp(SPRITE Sp, signed char X, signed char Y);
void draw_sprite_2bpp(SPRITE Sp, signed char X, signed char Y);
void fetch_tile_from_tilemap_1bpp(unsigned short N);
void fetch_tile_from_tilemap_2bpp(unsigned short N);
void draw_tile_1bpp(signed char X, signed char Y);
void draw_tile_2bpp(signed char X, signed char Y);
void SYM_H_1bpp(void);
void SYM_V_1bpp(void);
void SYM_H_2bpp(void);
void SYM_V_2bpp(void);
void draw_tilemap_1bpp(void);
void draw_tilemap_2bpp(void);
void draw_rectangle(int X, int Y, unsigned char width_in_tiles, unsigned char height_in_tiles);
void draw_line(signed char x0, signed char y0, signed char x1, signed char y1);
void draw_char(char c, char X, char Y);
void draw_string(const char* str, char X, char Y);



#endif