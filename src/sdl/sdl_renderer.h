#ifndef SDL_RENDERER_H
#define SDL_RENDERER_H

#include "../renderer.h"

#include <SDL.h>

struct Sprite {
    SDL_Texture* texture;
    int width, height;
};

struct Renderer {
    SDL_Renderer* sdl_renderer;
    SDL_Color clear_color;
    char* asset_dir;

    Sprite* tile_sprite;
    int tile_sprite_horizontal_slices;
    int tile_sprite_vertical_slices;

    int screen_size_x, screen_size_y;
};

Renderer* sdl_renderer_create(SDL_Window* window, const char* asset_dir);

void sdl_renderer_begin(Renderer* renderer);
void sdl_renderer_end(Renderer* renderer);


#endif