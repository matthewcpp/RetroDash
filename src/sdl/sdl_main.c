#include <SDL.h>

#include "../camera.h"
#include "../filesystem.h"
#include "../level.h"
#include "../player.h"

#include "sdl_input.h"
#include "sdl_renderer.h"

int main(int argc, char** argv) {
    SDL_VideoInit(NULL);

    const int screen_width = 320, screen_height = 240;

    SDL_Window* window = SDL_CreateWindow("N64 Game Jam", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height,SDL_WINDOW_SHOWN);
    SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);

    filesystem_init(ASSET_DIRECTORY);
    Input* input = sdl_input_create();
    Renderer* renderer = sdl_renderer_create(window, ASSET_DIRECTORY);
    renderer_set_clear_color(renderer, 10, 7, 53);
    Camera* camera = camera_create(screen_width, screen_height);

    Level* level = level_create(renderer, camera);
    level_load(level, "/level01.level");

    Player* player = player_create(level, renderer, camera, input);
    camera_set_target(camera, &player->bounding_box);
    camera_set_safe_margins(camera, -3.0f, 3.0f);
    player_start(player);

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
            sdl_input_update(input);
            player_update(player, (float)time_delta / 1000.0f);
            camera_update(camera);
            if (input_button_is_down(player->_input, CONTROLLER_1, CONTROLLER_BUTTON_L)) {
                player_kill(player);
            }

            sdl_renderer_begin(renderer);
            level_draw(level);
            player_draw(player);
            sdl_renderer_end(renderer);

            last_update = now;
        }
        SDL_Delay(1);
    }

    level_destroy(level);
    camera_destroy(camera);
    sdl_input_destory(input);
    player_destroy(player);
    sdl_renderer_destroy(renderer);
    filesystem_uninit();

    SDL_Quit();

    return 0;
}