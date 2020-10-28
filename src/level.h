#ifndef LEVEL_H
#define LEVEL_H

#include "camera.h"
#include "renderer.h"

#include <stdint.h>



typedef enum {
    TILE_BIT_NONE = 0,
    TILE_BIT_SOLID = 1
} TileBits;

typedef struct {
    uint32_t sprite_index;
    uint32_t bits;
} Tile;

typedef struct {
    uint32_t palette_size;
    Tile* palette;
    Sprite* sprite;
    Point tile_size;
} TileSet;

typedef struct {
    uint32_t width;
    uint32_t height;
    char* name;
    float gravity;

    TileSet tile_set;
    uint8_t* _tile_map;

    Renderer* _renderer;
    Camera* _camera;
} Level;

Level* level_create(Renderer* renderer, Camera* camera);
int level_load(Level* level, const char* path);
Tile* level_get_tile(Level* level, int x, int y);
void level_draw(Level* level);

#endif