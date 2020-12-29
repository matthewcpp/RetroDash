#ifndef RENDERER_H
#define RENDERER_H

#include <stdint.h>

#include "rect.h"

/**
 * A function that will be called when a renderer error occurs.  Will receive a string with details about the error.
 */
typedef void(*RendererErrorFunc)(const char*);

typedef struct Sprite Sprite;
typedef struct Renderer Renderer;
typedef struct Font Font;

int sprite_width(Sprite* sprite);
int sprite_height(Sprite* sprite);

int sprite_horizontal_frame_size(Sprite* sprite);
int sprite_vertical_frame_size(Sprite* sprite);

int font_size(Font* font);

void renderer_get_screen_size(Renderer* renderer, Point* screen_size);
void renderer_draw_sprite(Renderer* renderer, Sprite* sprite, int x, int y);
void renderer_draw_sprite_frame(Renderer* renderer, Sprite* sprite, int frame, int x, int y);
void renderer_draw_scaled_sprite(Renderer* renderer, Sprite* sprite,  int x, int y, float scale_x, float scale_y, int frame);
void renderer_set_color(Renderer* renderer, int r, int g, int b, int a);
void renderer_draw_filled_rect(Renderer* renderer, Rect* rect);
void renderer_draw_grid(Renderer* renderer, int x_offset, int size);

/**
 * Sets the number of tile batches to allocate.
 */
void renderer_set_tile_batch_size(Renderer* renderer, int size);
void renderer_begin_tile_drawing(Renderer* renderer, Sprite* sprite);
void renderer_draw_tile(Renderer* renderer, int index, int x, int y);
void renderer_end_tile_drawing(Renderer* renderer);
void renderer_set_clear_color(Renderer* renderer, uint8_t r, uint8_t g, uint8_t b);

Sprite* renderer_load_sprite(Renderer* renderer, const char* sprite_base_path);
void renderer_destroy_sprite(Renderer* renderer, Sprite* sprite);

Font* renderer_load_font(Renderer* renderer, const char* font_base_path);
void renderer_destroy_font(Renderer* renderer, Font* font);
Sprite* renderer_create_text_sprite(Renderer* renderer, Font* font, const char* str);

void renderer_set_error_callback(Renderer* renderer, RendererErrorFunc func);

#endif