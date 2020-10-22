#ifndef RENDERER_H
#define RENDERER_H

#include "rect.h"

typedef struct Sprite Sprite;
typedef struct Renderer Renderer;

Sprite* renderer_get_sprite(Renderer* renderer, int index);
void renderer_draw_sprite(Renderer* renderer, Sprite* sprite, int x, int y, int frame);
void renderer_set_color(Renderer* renderer, int r, int g, int b, int a);
void renderer_draw_filled_rect(Renderer* renderer, Rect* rect);

void renderer_begin_tile_drawing(Renderer* renderer);
void renderer_draw_tile(Renderer* renderer, int index, int x, int y);
void renderer_end_tile_drawing(Renderer* renderer);

#endif