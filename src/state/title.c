#include "title.h"

#include <stdlib.h>

StateTitle* state_title_create(Renderer* renderer, Input* input){
    StateTitle* state = malloc(sizeof(StateTitle));

    state->_input = input;
    state->_renderer = renderer;
    state->transition = GAME_STATE_NONE;

    state->_title_sprite = NULL;
    state->_title_sprite = renderer_load_sprite(state->_renderer, "/title");
    state->_character_top = renderer_load_sprite(state->_renderer, "/title_idle_top");
    state->_character_bottom = renderer_load_sprite(state->_renderer, "/title_idle_bottom");
    state->_platform = renderer_load_sprite(state->_renderer, "/title_platform");

    animation_player_init(&state->_animation);
    animation_player_load(&state->_animation, "/title_idle.animation");
    state->_animation.frame_time = 1.0f;
    animation_player_set_current(&state->_animation, 0, 1);

    return state;
}

void state_title_destroy(StateTitle* state){
    renderer_destroy_sprite(state->_renderer, state->_title_sprite);
    renderer_destroy_sprite(state->_renderer, state->_character_top);
    renderer_destroy_sprite(state->_renderer, state->_character_bottom);
    renderer_destroy_sprite(state->_renderer, state->_platform);

    free(state);
}

void state_title_update(StateTitle* state, float time_delta){
    animation_player_update(&state->_animation, time_delta);

    if (input_button_is_down(state->_input, CONTROLLER_1, CONTROLLER_BUTTON_START))
        state->transition = GAME_STATE_PLAYING;
}

#define CHARACTER_SCALE 1.25f

static void draw_character(StateTitle* state, Point* screen_size) {
    int platform_top = screen_size->y - (int)(sprite_vertical_frame_size(state->_platform) * CHARACTER_SCALE);
    int character_slice_height = (int)(sprite_vertical_frame_size(state->_character_top) * CHARACTER_SCALE);

    renderer_draw_scaled_sprite(state->_renderer, state->_platform,
                                0, platform_top,
                                1.5f, CHARACTER_SCALE,
                                0);

    renderer_draw_scaled_sprite(state->_renderer, state->_character_bottom,
                                25, platform_top - character_slice_height,
                                CHARACTER_SCALE, CHARACTER_SCALE,
                                state->_animation.frame);

    renderer_draw_scaled_sprite(state->_renderer, state->_character_top,
                                25, platform_top - 2 * character_slice_height,
                                CHARACTER_SCALE, CHARACTER_SCALE,
                                state->_animation.frame);
}

void state_title_draw(StateTitle* state){
    Point screen_size;
    renderer_get_screen_size(state->_renderer, &screen_size);

    renderer_set_color(state->_renderer, 33, 7, 58, 255);
    renderer_draw_grid(state->_renderer);

    int title_x = (screen_size.x / 2) - (sprite_width(state->_title_sprite) / 2);
    int title_y = 10;

    renderer_draw_sprite(state->_renderer, state->_title_sprite, title_x, title_y);

    draw_character(state, &screen_size);
}