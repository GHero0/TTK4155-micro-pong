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
void draw_sprite_2bpp(SPRITE Sp, int X, int Y, unsigned char wrap);




#endif