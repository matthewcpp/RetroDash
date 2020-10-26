#include <SDL.h>

#include "../filesystem.h"
#include "sdl_input.h"
#include "sdl_renderer.h"

#include "../level.h"

#include <stdio.h>

int main(int argc, char** argv) {
    SDL_VideoInit(NULL);

    const int screen_width = 320, screen_height = 240;

    SDL_Window* window = SDL_CreateWindow("N64 Game Jam", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height,SDL_WINDOW_SHOWN);
    SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);

    filesystem_init(ASSET_DIRECTORY);
    Input* input = sdl_input_create();
    Renderer* renderer = sdl_renderer_create(window, ASSET_DIRECTORY);
    renderer_set_clear_color(renderer, 10, 7, 53);

    Level* level = level_create(renderer);
    level_load(level, "/level01.level");

    SDL_Event event;
    int keep_going = 1;
    Uint32 last_update, now, time_delta;
    last_update = SDL_GetTicks();

    while (keep_going) {
        while(SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    keep_going = 0;
                    break;
            }
        }

        now = SDL_GetTicks();
        time_delta = now - last_update;
        if (time_delta >= 32) {
            sdl_input_update(input);

            sdl_renderer_begin(renderer);
            level_draw(level);
            sdl_renderer_end(renderer);

            last_update = now;
        }
        SDL_Delay(1);
    }

    return 0;
}