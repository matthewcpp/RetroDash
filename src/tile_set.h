#ifndef TILE_SET_H
#define TILE_SET_H

#include "renderer.h"

typedef enum {
    TILE_TYPE_NONE = 0,
    TILE_TYPE_SOLID = 1,
    TILE_TYPE_TRIANGLE = 2,
    TILE_TYPE_TUNNEL = 3,
    TILE_TYPE_BRICK = 4,
    TILE_TYPE_HALF = 5
} TileType;

typedef struct {
    uint32_t sprite_index;
    uint32_t type;
} Tile;

typedef struct {
    uint32_t palette_size;
    Tile* palette;
    Sprite* sprite;
    Point tile_size;
    uint32_t debris_index;
} TileSet;

int tile_set_load(TileSet* tile_set, const char* path, Renderer* renderer);
void tile_set_clear(TileSet* tile_set, Renderer* renderer);

#endif