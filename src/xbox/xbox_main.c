#include <hal/debug.h>
#include <hal/xbox.h>
#include <hal/video.h>
#include <windows.h>

#include "xbox_audio.h"

#include "../filesystem.h"
#include "../sdl/sdl_input.h"
#include "../sdl/sdl_renderer.h"
#include "../game.h"

#include <SDL.h>

static void xbox_fatal_error(const char* description)
{
    debugPrint("Fatal Error: %s\n", description);
    Sleep(5000);
    XReboot();
}

#define ASSET_DIR "D:\\build_sdl"

#define XBOX_SCREEN_WIDTH 640
#define XBOX_SCREEN_HEIGHT 480

#define GAME_SCREEN_WIDTH 320
#define GAME_SCREEN_HEIGHT 240

int main(int argc, char** argv) {
    XVideoSetMode(XBOX_SCREEN_WIDTH, XBOX_SCREEN_HEIGHT, 32, REFRESH_DEFAULT);

    SDL_VideoInit(NULL);
    SDL_Window* window = SDL_CreateWindow("Retro Dash", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
    SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);

    filesystem_init(ASSET_DIR);
    Input* input = sdl_input_create();
    Renderer* renderer = sdl_renderer_create(window, ASSET_DIR);
    Audio* audio = xbox_audio_create(ASSET_DIR);

    Game* game = game_create(audio, input, renderer);

    SDL_Event event;
    int keep_going = 1;
    Uint32 last_update, now, time_delta;

    // first call to poll event can take some time as systems are initialized.
    // prime it here before tight update loop
    SDL_PollEvent(&event);

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
            float update_time = (float)time_delta / 1000.0f;
            sdl_input_update(input);
            game_update(game, update_time);

            sdl_renderer_begin(renderer);
            game_draw(game);
            sdl_renderer_end(renderer);

            last_update = now;
        }
        SDL_Delay(1);
    }

    game_destroy(game);
    sdl_input_destory(input);
    sdl_renderer_destroy(renderer);
    xbox_audio_destroy(audio);
    filesystem_uninit();

    SDL_Quit();

    return 0;
}