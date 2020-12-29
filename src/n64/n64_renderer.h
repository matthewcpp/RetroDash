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
    display_context_t display_context;
    uint32_t clear_color;
    uint32_t primitive_color;
    Sprite* tile_sprite;
    TileBatch** tile_batches;
    SoftwareTileBatch software_tiles;
    int tile_batch_count;
    DrawMode draw_mode;
    Point screen_size;
    RendererErrorFunc error_callback;
};

Renderer* n64_renderer_create(int screen_width, int screen_height);
void n64_renderer_draw_software_tiles(Renderer* renderer);

#endif