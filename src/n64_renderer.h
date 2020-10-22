#ifndef N64_RENDERER_H
#define N64_RENDERER_H

#include "batch.h"
#include "renderer.h"

#include <libdragon.h>

#define BATCH_COUNT 4

struct Sprite {
	sprite_t* libdragon_sprite;
};

struct Renderer {
	Sprite* sprites[2];
	TileBatch* tile_batches[BATCH_COUNT];
};

Renderer* n64_renderer_create();

Sprite* n64_rendrer_load_sprite(Renderer* renderer, const char* path);
void n64_renderer_load_sprites(Renderer* renderer);

#endif