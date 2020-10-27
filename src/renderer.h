#ifndef RENDERER_H
#define RENDERER_H

#include <stdint.h>

#include "rect.h"

typedef struct Sprite Sprite;
typedef struct Renderer Renderer;

int sprite_width(Sprite* sprite);
int sprite_height(Sprite* sprite);

int sprite_horizontal_frame_size(Sprite* sprite);
int sprite_vertical_frame_size(Sprite* sprite);

void renderer_draw_sprite(Renderer* renderer, Sprite* sprite, int x, int y, int frame);
void renderer_set_color(Renderer* renderer, int r, int g, int b, int a);
void renderer_draw_filled_rect(Renderer* renderer, Rect* rect);

void renderer_begin_tile_drawing(Renderer* renderer, Sprite* sprite);
void renderer_draw_tile(Renderer* renderer, int index, int x, int y);
void renderer_end_tile_drawing(Renderer* renderer);
void renderer_set_clear_color(Renderer* renderer, uint8_t r, uint8_t g, uint8_t b);

Sprite* renderer_load_sprite(Renderer* renderer, const char* sprite_base_path);

#endif