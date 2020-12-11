#include "n64_input.h"

#include <stdlib.h>
#include <string.h>

Input* n64_input_create() {
    Input* input = malloc(sizeof(Input));

    memset(input, 0, sizeof(Input));

    return input;
}

#define AXIS_MIN_VAL -127.0f
#define AXIS_MAX_VAL 127.0f
#define AXIS_MAPPED_MIN -1.0f
#define AXIS_MAPPED_MAX 1.0f

static float map_controller_axis_value(int value) {
    float t = ((float)value - AXIS_MIN_VAL) /  (AXIS_MAX_VAL - AXIS_MIN_VAL);
    float result =  AXIS_MAPPED_MIN + t * (AXIS_MAPPED_MAX - AXIS_MAPPED_MIN);
    return result;
}

void n64_input_update(Input* input) {
    input->previous_state = input->current_state;
    input->axis_prev = input->axis_current;

    controller_read(&input->current_state);
    input->axis_current.x = map_controller_axis_value(input->current_state.c[0].x);
    input->axis_current.y = map_controller_axis_value(input->current_state.c[0].y);
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

        case CONTROLLER_BUTTON_L:
            return controller_state->c[controller].L;

        case CONTROLLER_BUTTON_R:
            return controller_state->c[controller].R;

        case CONTROLLER_BUTTON_START:
            return controller_state->c[controller].start;

        default:
            return 0;
    }
}


int input_button_is_down(Input* input, ControllerIndex controller, ControllerButton button) {
    return get_input_value(&input->current_state, controller, button) && !get_input_value(&input->previous_state, controller, button);
}

int input_button_is_up(Input* input, ControllerIndex controller, ControllerButton button) {
    return 0; //return get_input_value(&input->buttons_up, controller, button);
}

int input_button_is_held(Input* input, ControllerIndex controller, ControllerButton button) {
    return 0; //return get_input_value(&input->buttons_held, controller, button);
}

void input_axis_values(Input* input, Vec2* current, Vec2* prev) {
    *current = input->axis_current;
    *prev = input->axis_prev;
}
