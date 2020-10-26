#include "n64_renderer.h"
#include "n64_input.h"
#include "../level.h"

#include <libdragon.h>

#include "../filesystem.h"

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>


int main(void)
{
    init_interrupts();
    display_init( RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE );
    filesystem_init(NULL);
    rdp_init();
    controller_init();
    timer_init();

    Renderer* renderer = n64_renderer_create();
    renderer_set_clear_color(renderer, 10, 7, 53);
    Input* input = n64_input_create();

    Level* level = level_create(renderer);
    int result = level_load(level, "/level01.level");

    if (result == 0)
        renderer_set_clear_color(renderer, 255, 0, 0);

    char message[100];
    sprintf(message, "%s: %ld h: %ld", level->name, level->width, level->height);


    //unsigned long prev_time = get_ticks_ms();


    /* Main loop test */
    while(1) 
    {
        //unsigned long current_time = get_ticks_ms();
        //float time_delta = (current_time - prev_time) / 1000.0f;

        n64_input_update(input);

        static display_context_t disp = 0;

        /* Grab a render buffer */
        while( !(disp = display_lock()) );
       
        /*Fill the screen */
        graphics_fill_screen( disp, renderer->clear_color);

        graphics_set_color( 0x0, 0xFFFFFFFF );
        graphics_draw_text( disp, 20, 20, message);

        /* Assure RDP is ready for new commands */
        rdp_sync( SYNC_PIPE );

        /* Remove any clipping windows */
        rdp_set_default_clipping();

        /* Attach RDP to display */
        rdp_attach_display( disp );

        rdp_sync( SYNC_PIPE );

        level_draw(level);

        /* Inform the RDP we are finished drawing and that any pending operations should be flushed */
        rdp_sync( SYNC_PIPE );
        rdp_detach_display();

        /* Force backbuffer flip */
        display_show(disp);

        //prev_time = current_time;
    }
}
