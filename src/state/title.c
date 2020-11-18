#include "title.h"

#include <stdlib.h>

StateTitle* state_title_create(Renderer* renderer, Input* input){
    StateTitle* state = malloc(sizeof(StateTitle));

    state->_input = input;
    state->_renderer = renderer;
    state->transition = GAME_STATE_NONE;

    return state;
}

void state_title_destroy(StateTitle* state){
    free(state);
}

void state_title_update(StateTitle* state, float time_delta){
    if (input_button_is_down(state->_input, CONTROLLER_1, CONTROLLER_BUTTON_START))
        state->transition = GAME_STATE_PLAYING;
}

void state_title_draw(StateTitle* state){
    (void)state;
}