/**
 * @file sprites.h
 * @brief Sprites/tiles handling and some usefull prints for the screen
 */

#ifndef SPRITES_H
#define SPRITES_H

// =============================================================================
// INCLUDES
// =============================================================================

// Personal headers
#include "types.h"

// =============================================================================
// FUNCTION DECLARATIONS
// =============================================================================

void draw_sprite_1bpp(Sprite Sp, signed char X, signed char Y);
void draw_sprite_2bpp(Sprite Sp, signed char X, signed char Y);
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
void draw_rectangle(int X, int Y, unsigned char width, unsigned char height);
void draw_line(signed char x, signed char y, unsigned char length, char direction, char stride);
void draw_string(const char* str, char X, char Y);

#endif