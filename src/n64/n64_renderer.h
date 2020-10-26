#ifndef N64_RENDERER_H
#define N64_RENDERER_H

#include "batch.h"
#include "../renderer.h"

#include <libdragon.h>

#define BATCH_COUNT 4

struct Sprite {
    sprite_t* libdragon_sprite;
};

typedef enum {
    DDRAW_MODE_UNSPECIFIED,
    DRAW_MODE_TEXTURED,
    DRAW_MODE_FILLED
} DrawMode;

struct Renderer {
    uint32_t clear_color;
    Sprite* tile_sprite;
    TileBatch* tile_batches[BATCH_COUNT];
    DrawMode draw_mode;
};

Renderer* n64_renderer_create();

#endif