/**
 * @file images.h
 * @brief Passing tilemaps and font_spacing to sprites
 */

#ifndef IMAGES_H
#define IMAGES_H

#include "types.h"
#include <avr/pgmspace.h>

extern const unsigned char PROGMEM tilemap_1bpp[1024];
extern const unsigned char PROGMEM tilemap_2bpp[1024];
extern const unsigned char PROGMEM font_spacing[70];

extern Sprite logo1bpp;
extern Sprite logo1bpp_big;

#endif