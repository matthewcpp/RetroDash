#include "sdl_renderer.h"

#include "../filesystem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL_image.h>

Renderer* sdl_renderer_create(SDL_Window* window, const char* asset_dir) {
    Renderer* renderer = malloc(sizeof(Renderer));
    size_t asset_dir_size = strlen(asset_dir);
    renderer->asset_dir = malloc(asset_dir_size + 1);
    strcpy(renderer->asset_dir, asset_dir);

    renderer->sdl_renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_GetRendererOutputSize(renderer->sdl_renderer, &renderer->screen_size_x, &renderer->screen_size_y);

    renderer_set_clear_color(renderer, 255, 255, 255);
    renderer_end_tile_drawing(renderer);

    return renderer;
}

Sprite* renderer_load_sprite(Renderer* renderer, const char* sprite_base_path) {
    char* path_buffer = malloc(strlen(renderer->asset_dir) + strlen(sprite_base_path) + 12);
    sprintf(path_buffer, "%s%s.png", renderer->asset_dir, sprite_base_path);

    SDL_Surface* surface = IMG_Load(path_buffer);
    if (!surface) return NULL;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer->sdl_renderer, surface);
    SDL_FreeSurface(surface);

    Sprite* sprite = malloc(sizeof(Sprite));
    sprite->texture = texture;
    SDL_QueryTexture(sprite->texture, NULL, NULL, &sprite->width, &sprite->height);

    free(path_buffer);

    return sprite;
}

void renderer_set_color(Renderer* renderer, int r, int g, int b, int a) {
    SDL_SetRenderDrawColor(renderer->sdl_renderer, r, g, b, a);
}

void renderer_draw_filled_rect(Renderer* renderer, Rect* rect) {
    SDL_RenderFillRect(renderer->sdl_renderer, (SDL_Rect*)rect);
}

void renderer_begin_tile_drawing(Renderer* renderer, Sprite* sprite, int horizontal_slices, int vertical_slices) {
    renderer->tile_sprite = sprite;
    renderer->tile_sprite_horizontal_slices = horizontal_slices;
    renderer->tile_sprite_vertical_slices = vertical_slices;
}

static void _draw_sprite(Renderer* renderer, Sprite* sprite, int index, int size_x, int size_y, int dst_x, int dst_y) {
    SDL_Rect source_rect;
    source_rect.x = (index % renderer->tile_sprite_horizontal_slices) * size_x;
    source_rect.y = (index / renderer->tile_sprite_vertical_slices) * size_y;
    source_rect.w = size_x;
    source_rect.h = size_y;

    SDL_Rect dest_rect;
    dest_rect.x = dst_x;
    dest_rect.y = dst_y;
    dest_rect.w = size_x;
    dest_rect.h = size_y;

    SDL_RenderCopy(renderer->sdl_renderer, renderer->tile_sprite->texture, &source_rect, &dest_rect);
}

void renderer_draw_sprite(Renderer* renderer, Sprite* sprite, int x, int y, int frame) {
    _draw_sprite(renderer, sprite, frame, sprite->width, sprite->height, x, y);
}

void renderer_draw_tile(Renderer* renderer, int index, int x, int y) {
    _draw_sprite(renderer, renderer->tile_sprite, index, renderer->tile_sprite->width, renderer->tile_sprite->height, x, y);
}

void renderer_end_tile_drawing(Renderer* renderer) {
    renderer->tile_sprite = NULL;
    renderer->tile_sprite_horizontal_slices = 0;
    renderer->tile_sprite_vertical_slices = 0;
}

void sdl_renderer_begin(Renderer * renderer) {
    SDL_SetRenderDrawColor(renderer->sdl_renderer, renderer->clear_color.r, renderer->clear_color.g, renderer->clear_color.b, 255);
    SDL_RenderClear(renderer->sdl_renderer);
}

void sdl_renderer_end(Renderer * renderer) {
    SDL_RenderPresent(renderer->sdl_renderer);
}

void renderer_set_clear_color(Renderer* renderer, uint8_t r, uint8_t g, uint8_t b) {
    renderer->clear_color.r = r;
    renderer->clear_color.g = g;
    renderer->clear_color.b = b;
    renderer->clear_color.a = 255;
}

int sprite_width(Sprite* sprite) {
    return sprite->width;
}

int sprite_height(Sprite* sprite) {
    return sprite->height;
}