#ifndef LEVEL_H
#define LEVEL_H

#include "renderer.h"

#include <stdint.h>

#define TILE_EMPTY UINT8_MAX

typedef enum {
    TILE_BIT_NONE = 0,
    TILE_BIT_SOLID = 1
} TileBits;

typedef struct {
    int sprite_index;
    unsigned int bits;
} Tile;

typedef struct {
    uint32_t palette_size;
    Tile* palette;
    Sprite* sprite;
    uint32_t sprite_horizontal_slices;
    uint32_t sprite_vertical_slices;
} TileSet;

typedef struct {
    Renderer* _renderer;
    uint32_t width;
    uint32_t height;
    char* name;

    TileSet tile_set;
    uint8_t* _tile_map;
} Level;

Level* level_create(Renderer* renderer);
int level_load(Level* level, const char* path);
int level_set_tile(Level* level, int x, int y, int tile);
void level_draw(Level* level);

#endif