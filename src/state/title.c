#include "title.h"

#include <stdlib.h>

StateTitle* state_title_create(Renderer* renderer, Input* input){
    StateTitle* state = malloc(sizeof(StateTitle));

    state->_input = input;
    state->_renderer = renderer;
    state->transition = GAME_STATE_NONE;

    state->_title_sprite = NULL;
    state->_title_sprite = renderer_load_sprite(state->_renderer, "/title");

    return state;
}

void state_title_destroy(StateTitle* state){
    if (state->_title_sprite)
        renderer_destroy_sprite(state->_renderer, state->_title_sprite);

    free(state);
}

void state_title_update(StateTitle* state, float time_delta){
    if (input_button_is_down(state->_input, CONTROLLER_1, CONTROLLER_BUTTON_START))
        state->transition = GAME_STATE_PLAYING;
}

void state_title_draw(StateTitle* state){
    Point screen_size;
    renderer_get_screen_size(state->_renderer, &screen_size);

    renderer_set_color(state->_renderer, 33, 7, 58, 255);
    renderer_draw_grid(state->_renderer);
    //draw_grid(state, &screen_size);

    if (state->_title_sprite == NULL)  return;
    int title_x = (screen_size.x / 2) - (sprite_width(state->_title_sprite) / 2);
    int title_y = 10;

    renderer_draw_sprite(state->_renderer, state->_title_sprite, title_x, title_y);
}