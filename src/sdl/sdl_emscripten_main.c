#include <SDL2/SDL.h>

#include "../filesystem.h"
#include "../game.h"

#include "sdl_audio.h"
#include "sdl_input.h"
#include "sdl_renderer.h"

#include <emscripten.h>
#include <emscripten/html5.h>

typedef struct {
    Uint32 last_update_time;
    Input* input;
    Renderer* renderer;
    Audio* audio;
    Game* game;
} EmscriptenGameContext;

void emscripten_game_context_update(void* arg) {

    EmscriptenGameContext* context = (EmscriptenGameContext*)arg;
    SDL_Event event;
    while (SDL_PollEvent(&event));

    Uint32 now = SDL_GetTicks();
    Uint32 time_delta = now - context->last_update_time;

    float update_time = (float)time_delta / 1000.0f;
    sdl_input_update(context->input);
    game_update(context->game, update_time);

    sdl_renderer_begin(context->renderer);
    game_draw(context->game);
    sdl_renderer_end(context->renderer);

    context->last_update_time = now;
}

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);
    EmscriptenGameContext context;

    const int screen_width = 320, screen_height = 240;

    SDL_Window* window = SDL_CreateWindow("Retro Dash", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height,SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);

    filesystem_init(ASSET_DIRECTORY);

    context.input = sdl_input_create();
    context.renderer = sdl_renderer_create(window, ASSET_DIRECTORY);
    context.audio = sdl_audio_create();
    context.game = game_create(context.audio, context.input, context.renderer);

    emscripten_set_main_loop_arg(emscripten_game_context_update, &context, -1, 1);
    puts("after main loop");
    return 0;
}