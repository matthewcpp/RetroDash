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
    SDL_GetRendererOutputSize(renderer->sdl_renderer, &renderer->screen_size.x, &renderer->screen_size.y);

    renderer_set_clear_color(renderer, 255, 255, 255);
    renderer_end_tile_drawing(renderer);

    return renderer;
}

void renderer_get_screen_size(Renderer* renderer, Point* screen_size) {
    screen_size->x = renderer->screen_size.x;
    screen_size->y = renderer->screen_size.y;
}

Sprite* renderer_load_sprite(Renderer* renderer, const char* sprite_base_path) {
    char* path_buffer = malloc(strlen(renderer->asset_dir) + strlen(sprite_base_path) + 12); //.sprite
    sprintf(path_buffer, "%s%s.png", renderer->asset_dir, sprite_base_path);

    SDL_Surface* surface = IMG_Load(path_buffer);
    if (!surface) return NULL;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer->sdl_renderer, surface);
    SDL_FreeSurface(surface);

    Sprite* sprite = malloc(sizeof(Sprite));
    sprite->texture = texture;
    SDL_QueryTexture(sprite->texture, NULL, NULL, &sprite->width, &sprite->height);

    sprintf(path_buffer, "%s%s.sprite", renderer->asset_dir, sprite_base_path);
    FILE* sprite_info_file = fopen(path_buffer, "rb");
    fread(&sprite->horizontal_slices, sizeof(uint32_t), 1, sprite_info_file);
    fread(&sprite->vertical_slices, sizeof(uint32_t), 1, sprite_info_file);
    fclose(sprite_info_file);

    free(path_buffer);

    return sprite;
}

void renderer_set_color(Renderer* renderer, int r, int g, int b, int a) {
    SDL_SetRenderDrawColor(renderer->sdl_renderer, r, g, b, a);
}

void renderer_draw_filled_rect(Renderer* renderer, Rect* rect) {
    SDL_RenderFillRect(renderer->sdl_renderer, (SDL_Rect*)rect);
}

void renderer_set_tile_batch_size(Renderer* renderer, int size) {
    (void)renderer;
    (void)size;
}

void renderer_begin_tile_drawing(Renderer* renderer, Sprite* sprite) {
    renderer->tile_sprite = sprite;
    renderer->tile_size_x = sprite_horizontal_frame_size(sprite);
    renderer->tile_size_y = sprite_vertical_frame_size(sprite);
}

static void _draw_sprite(Renderer* renderer, Sprite* sprite, int index, int size_x, int size_y, int dst_x, int dst_y) {
    SDL_Rect source_rect;
    source_rect.x = (index % sprite->horizontal_slices) * size_x;
    source_rect.y = (index / sprite->horizontal_slices) * size_y;
    source_rect.w = size_x;
    source_rect.h = size_y;

    SDL_Rect dest_rect;
    dest_rect.x = dst_x;
    dest_rect.y = dst_y;
    dest_rect.w = size_x;
    dest_rect.h = size_y;

    SDL_RenderCopy(renderer->sdl_renderer, sprite->texture, &source_rect, &dest_rect);
}

void renderer_draw_sprite(Renderer* renderer, Sprite* sprite, int x, int y, int frame) {
    _draw_sprite(renderer, sprite, frame, sprite_horizontal_frame_size(sprite), sprite_vertical_frame_size(sprite), x, y);
}

void renderer_draw_scaled_sprite(Renderer* renderer, Sprite* sprite,  int x, int y, float scale_x, float scale_y, int frame) {
    SDL_Rect source_rect;
    source_rect.x = (frame % sprite->horizontal_slices) * sprite->width;
    source_rect.y = (frame / sprite->vertical_slices) * sprite->height;
    source_rect.w = sprite_horizontal_frame_size(sprite);
    source_rect.h = sprite_vertical_frame_size(sprite);

    SDL_Rect dest_rect;
    dest_rect.x = x;
    dest_rect.y = y;
    dest_rect.w = sprite->width * scale_x;
    dest_rect.h = sprite->height * scale_y;

    SDL_RenderCopy(renderer->sdl_renderer, sprite->texture, &source_rect, &dest_rect);
}

void renderer_draw_tile(Renderer* renderer, int index, int x, int y) {
    _draw_sprite(renderer, renderer->tile_sprite, index, renderer->tile_size_x, renderer->tile_size_y, x, y);
}

void renderer_end_tile_drawing(Renderer* renderer) {
    renderer->tile_sprite = NULL;
    renderer->tile_size_x = 0;
    renderer->tile_size_y = 0;
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

int sprite_horizontal_frame_size(Sprite* sprite) {
    return sprite->width / sprite->horizontal_slices;
}

int sprite_vertical_frame_size(Sprite* sprite) {
    return sprite->height / sprite->vertical_slices;
}
