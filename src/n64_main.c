#include "n64_renderer.h"
#include "n64_input.h"
#include "level.h"
#include "player.h"

#include <libdragon.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>


int main(void)
{
    init_interrupts();
    display_init( RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE );
    dfs_init( DFS_DEFAULT_LOCATION );
    rdp_init();
    controller_init();
    timer_init();

    Renderer* renderer = n64_renderer_create();
    n64_renderer_load_sprites(renderer);

    Input* input = n64_input_create();

    Level* level = level_create(renderer);
    Player* player = player_create(level, input, renderer);
    player->pos_x = 75;
    player->pos_y = 150;

    char message[100];

    unsigned long prev_time = get_ticks_ms();


    /* Main loop test */
    while(1) 
    {
        unsigned long current_time = get_ticks_ms();
        float time_delta = (current_time - prev_time) / 1000.0f;

        n64_input_update(input);

        if (input_button_is_down(input, CONTROLLER_1, CONTROLLER_BUTTON_L))
            player->jump_velocity -= 5.0f;

        if (input_button_is_down(input, CONTROLLER_1, CONTROLLER_BUTTON_R))
            player->jump_velocity += 5.0f;

        if (input_button_is_down(input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_LEFT))
            level->gravity -= 5.0f;

        if (input_button_is_down(input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_RIGHT))
            level->gravity += 5.0f;

        level_update(level, time_delta);
        player_update(player, time_delta);

        static display_context_t disp = 0;

        /* Grab a render buffer */
        while( !(disp = display_lock()) );
       
        /*Fill the screen */
        graphics_fill_screen( disp, 0xFFFFFFFF );

        /* Set the text output color */
        graphics_set_color( 0x0, 0xFFFFFFFF );

        sprintf(message, "jump: %.2f gravity: %.2f", player->jump_velocity, level->gravity);
        graphics_draw_text( disp, 30, 0, message);

        /* Assure RDP is ready for new commands */
        rdp_sync( SYNC_PIPE );

        /* Remove any clipping windows */
        rdp_set_default_clipping();


        /* Attach RDP to display */
        rdp_attach_display( disp );

        rdp_enable_primitive_fill ();
        level_draw(level);

        rdp_enable_texture_copy();
        player_draw(player);

        /* Inform the RDP we are finished drawing and that any pending operations should be flushed */
        rdp_detach_display();

        /* Force backbuffer flip */
        display_show(disp);

        prev_time = current_time;
    }
}
