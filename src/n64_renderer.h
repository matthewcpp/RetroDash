#ifndef N64_RENDERER_H
#define N64_RENDERER_H

#include "renderer.h"

#include <libdragon.h>

struct Sprite {
	sprite_t* libdragon_sprite;
};

struct Renderer {
	Sprite* sprites[1];
};

Renderer* n64_renderer_create();

Sprite* n64_rendrer_load_sprite(Renderer* renderer, const char* path);
void n64_renderer_load_sprites(Renderer* renderer);

#endif