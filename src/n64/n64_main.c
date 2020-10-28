#include "n64_renderer.h"
#include "n64_input.h"

#include "../camera.h"
#include "../filesystem.h"
#include "../level.h"
#include "../player.h"
#include "../renderer.h"

#include <libdragon.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

int main(void)
{
    init_interrupts();
    display_init( RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE );
    filesystem_init(NULL);
    rdp_init();
    controller_init();
    timer_init();

    Input* input = n64_input_create();

    Renderer* renderer = n64_renderer_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    renderer_set_clear_color(renderer, 10, 7, 53);

    Camera* camera = camera_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    Level* level = level_create(renderer, camera);
    level_load(level, "/level01.level");

    Player* player = player_create(level, renderer, camera, input);
    camera_set_target(camera, &player->position);
    camera_set_offset(camera, -3.0f, 8.0f);
    player_start(player);


    unsigned long prev_time = get_ticks_ms();


    /* Main loop test */
    while(1) 
    {
        unsigned long current_time = get_ticks_ms();
        float time_delta = (current_time - prev_time) / 1000.0f;
        (void)time_delta;

        n64_input_update(input);
        player_update(player, time_delta);
        if (input_button_is_down(player->_input, CONTROLLER_1, CONTROLLER_BUTTON_L)) {
            player_kill(player);
        }

        static display_context_t disp = 0;

        /* Grab a render buffer */
        while( !(disp = display_lock()) );
       
        /*Fill the screen */
        graphics_fill_screen( disp, renderer->clear_color);

        /* Assure RDP is ready for new commands */
        rdp_sync( SYNC_PIPE );

        /* Remove any clipping windows */
        rdp_set_default_clipping();

        /* Attach RDP to display */
        rdp_attach_display( disp );

        rdp_sync( SYNC_PIPE );

        level_draw(level);
        player_draw(player);

        /* Inform the RDP we are finished drawing and that any pending operations should be flushed */
        rdp_sync( SYNC_PIPE );
        rdp_detach_display();

        /* Force backbuffer flip */
        display_show(disp);

        prev_time = current_time;
    }
}
