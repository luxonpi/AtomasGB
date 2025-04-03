//AUTOGENERATED FILE FROM png2asset
#ifndef METASPRITE_gamebg_H
#define METASPRITE_gamebg_H

#include <stdint.h>
#include <gbdk/platform.h>
#include <gbdk/metasprites.h>

#define gamebg_TILE_ORIGIN 0
#define gamebg_TILE_W 8
#define gamebg_TILE_H 8
#define gamebg_WIDTH 160
#define gamebg_HEIGHT 144
#define gamebg_TILE_COUNT 192
#define gamebg_PALETTE_COUNT 2
#define gamebg_COLORS_PER_PALETTE 4
#define gamebg_TOTAL_COLORS 8
#define gamebg_MAP_ATTRIBUTES 0
extern const unsigned char gamebg_map[360];
#define gamebg_map_attributes gamebg_map

BANKREF_EXTERN(gamebg)

extern const palette_color_t gamebg_palettes[8];
extern const uint8_t gamebg_tiles[3072];

#endif
