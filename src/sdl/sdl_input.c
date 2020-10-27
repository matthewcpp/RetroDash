#include "sdl_input.h"

#include <stdlib.h>

typedef struct {
    int prev_state;
    int cur_state;
    int hold_time;
} rrSDLButtonInfo;

struct Input{
    SDL_GameController* controller;
    rrSDLButtonInfo controller_buttons[CONTROLLER_BUTTON_COUNT];
};

Input* sdl_input_create() {
    Input* input = calloc(1, sizeof(Input));

    if (SDL_NumJoysticks() > 0) {
        input->controller = SDL_GameControllerOpen(0);
    }

    return input;
}

void rr_sdl_input_update_button(Input* input, ControllerButton button, int value, int time) {
    rrSDLButtonInfo* button_info = &input->controller_buttons[button];
    button_info->prev_state = button_info->cur_state;
    button_info->cur_state = value;

    if (button_info->prev_state && button_info->cur_state)
        button_info->hold_time += time;
    else
        button_info->hold_time = 0;
}

static void sdl_update_controller(Input* input) {
    rr_sdl_input_update_button(input, CONTROLLER_BUTTON_A, SDL_GameControllerGetButton(input->controller, SDL_CONTROLLER_BUTTON_A ), 0);
}

static void sdl_update_keyboard(Input* input) {
    const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);
    rr_sdl_input_update_button(input, CONTROLLER_BUTTON_A, keyboard_state[SDL_SCANCODE_X], 0);
}

void sdl_input_update(Input* input) {
    if (input->controller)
        sdl_update_controller(input);
    else
        sdl_update_keyboard(input);
}

int input_button_is_down(Input* input, ControllerIndex controller, ControllerButton button) {
    return input->controller_buttons[button].prev_state == 0 && input->controller_buttons[button].cur_state != 0;
}

int input_button_is_up(Input* input, ControllerIndex controller, ControllerButton button) {
    return 0;
}

int input_button_is_held(Input* input, ControllerIndex controller, ControllerButton button) {
    return 0;
}
