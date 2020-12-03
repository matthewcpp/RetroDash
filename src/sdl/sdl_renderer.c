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

void sdl_renderer_destroy(Renderer* renderer) {
    SDL_DestroyRenderer(renderer->sdl_renderer);
    free(renderer->asset_dir);

    free(renderer);
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

void renderer_destroy_sprite(Renderer* renderer, Sprite* sprite) {
    SDL_DestroyTexture(sprite->texture);
    free(sprite);
    (void)renderer;
}

void renderer_set_color(Renderer* renderer, int r, int g, int b, int a) {
    SDL_SetRenderDrawColor(renderer->sdl_renderer, r, g, b, a);
}

void renderer_draw_filled_rect(Renderer* renderer, Rect* rect) {
    SDL_RenderFillRect(renderer->sdl_renderer, (SDL_Rect*)rect);
}

void renderer_draw_grid(Renderer* renderer, int x_offset, int size) {
    Point p0 = {0, size};
    Point p1;
    point_set(&p1, renderer->screen_size.x, size);

    while (p1.y < renderer->screen_size.y) {
        SDL_RenderDrawLine(renderer->sdl_renderer, p0.x, p0.y, p1.x, p1.y);
        p0.y += size;
        p1.y += size;
    }

    point_set(&p0, x_offset, 0);
    point_set(&p1, x_offset, renderer->screen_size.y);

    while (p1.x < renderer->screen_size.x) {
        SDL_RenderDrawLine(renderer->sdl_renderer, p0.x, p0.y, p1.x, p1.y);
        p0.x += size;
        p1.x += size;
    }
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

// TODO: Fix this as below
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

void renderer_draw_sprite(Renderer* renderer, Sprite* sprite, int x, int y) {
    SDL_Rect source_rect;
    source_rect.x = 0;
    source_rect.y = 0;
    source_rect.w = sprite->width;
    source_rect.h = sprite->height;

    SDL_Rect dest_rect;
    dest_rect.x = x;
    dest_rect.y = y;
    dest_rect.w = sprite->width;
    dest_rect.h = sprite->height;

    SDL_RenderCopy(renderer->sdl_renderer, sprite->texture, &source_rect, &dest_rect);
}

void renderer_draw_sprite_frame(Renderer* renderer, Sprite* sprite, int frame, int x, int y) {
    int horizontal_size = sprite_horizontal_frame_size(sprite);
    int vertical_size = sprite_vertical_frame_size(sprite);
    SDL_Rect source_rect;

    source_rect.x = (frame % sprite->horizontal_slices) * horizontal_size;
    source_rect.y = source_rect.y = (frame / sprite->horizontal_slices) * vertical_size;
    source_rect.w = horizontal_size;
    source_rect.h = vertical_size;

    SDL_Rect dest_rect;
    dest_rect.x = x;
    dest_rect.y = y;
    dest_rect.w = horizontal_size;
    dest_rect.h = vertical_size;

    SDL_RenderCopy(renderer->sdl_renderer, sprite->texture, &source_rect, &dest_rect);
}

void renderer_draw_scaled_sprite(Renderer* renderer, Sprite* sprite,  int x, int y, float scale_x, float scale_y, int frame) {
    int horizontal_size = sprite_horizontal_frame_size(sprite);
    int vertical_size = sprite_vertical_frame_size(sprite);
    SDL_Rect source_rect;

    if (sprite->horizontal_slices == 1)
        source_rect.x = 0;
    else
        source_rect.x = (frame % sprite->horizontal_slices) * horizontal_size;

    if (sprite->vertical_slices == 1)
        source_rect.y = 0;
    else
        source_rect.y = (frame / sprite->horizontal_slices) * vertical_size;

    source_rect.w = horizontal_size;
    source_rect.h = vertical_size;

    SDL_Rect dest_rect;
    dest_rect.x = x;
    dest_rect.y = y;
    dest_rect.w = horizontal_size * scale_x;
    dest_rect.h = vertical_size * scale_y;

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