#ifndef LEVEL_H
#define LEVEL_H

#include "camera.h"
#include "renderer.h"
#include "tile_set.h"

#include <stdint.h>

#define TILE_EMPTY UINT8_MAX



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
void level_destroy(Level* level);

int level_load(Level* level, const char* path);
Tile* level_get_tile(Level* level, int x, int y);
void level_draw(Level* level);
void level_set_tile(Level* level, int x, int y, uint8_t tile_palette_index);

#endif