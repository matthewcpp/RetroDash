#include "n64_audio.h"
#include "n64_input.h"
#include "n64_renderer.h"

#include "../filesystem.h"
#include "../game.h"

#include <libdragon.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define UPDATE_MS 16

int main(void)
{
    init_interrupts();
    display_init( RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE );
    filesystem_init(NULL);
    rdp_init();
    controller_init();
    timer_init();

    Audio* audio = n64_audio_create();
    Input* input = n64_input_create();
    Renderer* renderer = n64_renderer_create(SCREEN_WIDTH, SCREEN_HEIGHT);

    Game* game = game_create(audio, input, renderer);

    unsigned long prev_update_time = get_ticks_ms();
    while(1) 
    {
        n64_audio_update(audio);

        unsigned long current_update_time = get_ticks_ms();

        if (current_update_time < prev_update_time) {
            prev_update_time = current_update_time;
            continue;
        }

        unsigned long time_delta_ms =current_update_time - prev_update_time;

        if (time_delta_ms >= UPDATE_MS) {
            float time_delta = (float)time_delta_ms / 1000.0f;

            n64_audio_update(audio);
            n64_input_update(input);
            game_update(game, time_delta);

            prev_update_time = current_update_time;
        }

        while( !(renderer->display_context = display_lock()) );

        graphics_fill_screen( renderer->display_context, renderer->clear_color);
        rdp_sync( SYNC_PIPE );
        rdp_set_default_clipping();
        rdp_attach_display( renderer->display_context );
        rdp_sync( SYNC_PIPE );

        game_draw(game);

        rdp_sync( SYNC_PIPE );
        rdp_detach_display();

        n64_renderer_draw_software_tiles(renderer);
        display_show(renderer->display_context);
    }
}
