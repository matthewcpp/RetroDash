#include "sdl_input.h"

#include <stdlib.h>

struct Input{
    SDL_GameController* controller;
};

Input* sdl_input_create() {
    Input* input = malloc(sizeof(Input));

    return input;
}

void sdl_input_update(Input* input) {

}

int input_button_is_down(Input* input, ControllerIndex controller, ControllerButton button) {
    return 0;
}

int input_button_is_up(Input* input, ControllerIndex controller, ControllerButton button) {
    return 0;
}

int input_button_is_held(Input* input, ControllerIndex controller, ControllerButton button) {
    return 0;
}
