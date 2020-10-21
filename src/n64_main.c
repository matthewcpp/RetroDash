#include "n64_renderer.h"
#include "n64_input.h"
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

    Player* player = player_create(input, renderer);

    const char* message = "chris is cool";

    unsigned long prev_time = get_ticks_ms();


    /* Main loop test */
    while(1) 
    {
        unsigned long current_time = get_ticks_ms();
        float time_delta = (current_time - prev_time) / 1000.0f;
        n64_input_update(input);
        player_update(player, time_delta);

        static display_context_t disp = 0;

        /* Grab a render buffer */
        while( !(disp = display_lock()) );
       
        /*Fill the screen */
        graphics_fill_screen( disp, 0xFFFFFFFF );

        /* Set the text output color */
        graphics_set_color( 0x0, 0xFFFFFFFF );

        graphics_draw_text( disp, 30, 0, message);

        /* Assure RDP is ready for new commands */
        rdp_sync( SYNC_PIPE );

        /* Remove any clipping windows */
        rdp_set_default_clipping();

        /* Enable sprite display instead of solid color fill */
        rdp_enable_texture_copy();

        /* Attach RDP to display */
        rdp_attach_display( disp );

        player_draw(player);

        /* Inform the RDP we are finished drawing and that any pending operations should be flushed */
        rdp_detach_display();

        /* Force backbuffer flip */
        display_show(disp);

        prev_time = current_time;
    }
}
