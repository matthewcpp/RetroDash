#include "n64_input.h"

#include <stdlib.h>
#include <string.h>

Input* n64_input_create() {
    Input* input = malloc(sizeof(Input));

    memset(input, 0, sizeof(Input));

    return input;
}

void n64_input_update(Input* input) {
    controller_scan();

    input->buttons_down = get_keys_down();
    input->buttons_up = get_keys_up();
    input->buttons_held = get_keys_held();
}

int get_input_value(struct controller_data* controller_state, ControllerIndex controller, ControllerButton button) {
    switch (button) {
        case CONTROLLER_BUTTON_DPAD_RIGHT:
            return controller_state->c[controller].right;

        case CONTROLLER_BUTTON_DPAD_LEFT:
            return controller_state->c[controller].left;

        case CONTROLLER_BUTTON_DPAD_UP:
            return controller_state->c[controller].up;

        case CONTROLLER_BUTTON_DPAD_DOWN:
            return controller_state->c[controller].down;

        case CONTROLLER_BUTTON_C_UP:
            return controller_state->c[controller].C_up;

        case CONTROLLER_BUTTON_C_DOWN:
            return controller_state->c[controller].C_down;

        case CONTROLLER_BUTTON_C_LEFT:
            return controller_state->c[controller].C_left;

        case CONTROLLER_BUTTON_C_RIGHT:
            return controller_state->c[controller].C_right;

        case CONTROLLER_BUTTON_A:
            return controller_state->c[controller].A;

        case CONTROLLER_BUTTON_B:
            return controller_state->c[controller].B;

        case CONTROLLER_BUTTON_Z:
            return controller_state->c[controller].Z;

        default:
            return 0;
    }
}


int input_button_is_down(Input* input, ControllerIndex controller, ControllerButton button) {
    return get_input_value(&input->buttons_down, controller, button);
}

int input_button_is_up(Input* input, ControllerIndex controller, ControllerButton button) {
    return get_input_value(&input->buttons_up, controller, button);
}

int input_button_is_held(Input* input, ControllerIndex controller, ControllerButton button) {
    return get_input_value(&input->buttons_held, controller, button);
}
