#ifndef IMAGES_H
#define IMAGES_H

#include <avr/pgmspace.h>

typedef struct
{
    char height_tile;
    char width_tile;
    const unsigned char *sprite_tiles;
    const unsigned char *trans_tiles;
} SPRITE;

extern SPRITE logo_1bpp;
extern SPRITE logo_2bpp;

extern const unsigned char PROGMEM tilemap_1bpp[1024];
extern const unsigned char PROGMEM tilemap_2bpp[1024];


#endif