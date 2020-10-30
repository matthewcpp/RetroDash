#ifndef N64_RENDERER_H
#define N64_RENDERER_H

#include "batch.h"
#include "../renderer.h"

#include <libdragon.h>

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
    TileBatch** tile_batches;
    int tile_batch_count;
    DrawMode draw_mode;
    Point screen_size;
};

Renderer* n64_renderer_create(int screen_width, int screen_height);

#endif