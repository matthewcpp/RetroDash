#include "sdl_input.h"

#include <stdlib.h>

typedef struct {
    int prev_state;
    int cur_state;
    int hold_time;
} SDLButtonInfo;

typedef struct {
    Vec2 prev_state;
    Vec2 current_state;
} SDLAxisInfo;

struct Input{
    SDL_GameController* controller;
    SDLButtonInfo controller_buttons[CONTROLLER_BUTTON_COUNT];
    SDLAxisInfo controller_axis;
};

Input* sdl_input_create() {
    Input* input = calloc(1, sizeof(Input));

    if (SDL_NumJoysticks() > 0) {
        input->controller = SDL_GameControllerOpen(0);
    }

    return input;
}

void sdl_input_destory(Input* input) {
    if (input->controller) {
        SDL_GameControllerClose(input->controller);
    }

    free(input);
}

void sdl_input_update_button(Input* input, ControllerButton button, int value, int time) {
    SDLButtonInfo* button_info = &input->controller_buttons[button];
    button_info->prev_state = button_info->cur_state;
    button_info->cur_state = value;

    if (button_info->prev_state && button_info->cur_state)
        button_info->hold_time += time;
    else
        button_info->hold_time = 0;
}

#define AXIS_MIN_VAL -32768.0f
#define AXIS_MAX_VAL 32767.0f
#define AXIS_MAPPED_MIN -1.0f
#define AXIS_MAPPED_MAX 1.0f

static float map_controller_axis_value(Sint16 value) {
    float t = ((float)value - AXIS_MIN_VAL) /  (AXIS_MAX_VAL - AXIS_MIN_VAL);
    float result =  AXIS_MAPPED_MIN + t * (AXIS_MAPPED_MAX - AXIS_MAPPED_MIN);
    return result;
}

static void sdl_update_controller(Input* input) {
    sdl_input_update_button(input, CONTROLLER_BUTTON_A, SDL_GameControllerGetButton(input->controller, SDL_CONTROLLER_BUTTON_A), 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_B,  SDL_GameControllerGetButton(input->controller, SDL_CONTROLLER_BUTTON_BACK), 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_Z, SDL_GameControllerGetAxis(input->controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT), 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_L, SDL_GameControllerGetButton(input->controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER), 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_R, SDL_GameControllerGetButton(input->controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER), 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_START, SDL_GameControllerGetButton(input->controller, SDL_CONTROLLER_BUTTON_START), 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_C_LEFT, SDL_GameControllerGetButton(input->controller, SDL_CONTROLLER_BUTTON_X), 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_C_UP, SDL_GameControllerGetButton(input->controller, SDL_CONTROLLER_BUTTON_Y), 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_C_RIGHT, SDL_GameControllerGetButton(input->controller, SDL_CONTROLLER_BUTTON_B), 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_DPAD_UP, SDL_GameControllerGetButton(input->controller, SDL_CONTROLLER_BUTTON_DPAD_UP), 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_DPAD_DOWN, SDL_GameControllerGetButton(input->controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN), 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_DPAD_LEFT, SDL_GameControllerGetButton(input->controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT), 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_DPAD_RIGHT, SDL_GameControllerGetButton(input->controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT), 0);

    input->controller_axis.prev_state = input->controller_axis.current_state;
    input->controller_axis.current_state.x = map_controller_axis_value(SDL_GameControllerGetAxis(input->controller, SDL_CONTROLLER_AXIS_LEFTX));
    input->controller_axis.current_state.y = -map_controller_axis_value(SDL_GameControllerGetAxis(input->controller, SDL_CONTROLLER_AXIS_LEFTY));
}

static void sdl_update_keyboard(Input* input) {
    const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);
    sdl_input_update_button(input, CONTROLLER_BUTTON_START, keyboard_state[SDL_SCANCODE_RETURN], 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_A, keyboard_state[SDL_SCANCODE_X], 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_B, keyboard_state[SDL_SCANCODE_C], 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_Z, keyboard_state[SDL_SCANCODE_Z], 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_L, keyboard_state[SDL_SCANCODE_A], 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_R, keyboard_state[SDL_SCANCODE_S], 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_C_UP, keyboard_state[SDL_SCANCODE_T], 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_C_DOWN, keyboard_state[SDL_SCANCODE_G], 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_C_LEFT, keyboard_state[SDL_SCANCODE_F], 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_C_RIGHT, keyboard_state[SDL_SCANCODE_H], 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_DPAD_UP, keyboard_state[SDL_SCANCODE_I], 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_DPAD_DOWN, keyboard_state[SDL_SCANCODE_K], 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_DPAD_LEFT, keyboard_state[SDL_SCANCODE_J], 0);
    sdl_input_update_button(input, CONTROLLER_BUTTON_DPAD_RIGHT, keyboard_state[SDL_SCANCODE_L], 0);
}

void input_axis_values(Input* input, Vec2* current, Vec2* prev) {
    *current = input->controller_axis.current_state;
    *prev = input->controller_axis.prev_state;
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
