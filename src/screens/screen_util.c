#include "screen_util.h"

#include "../rect.h"

#define AXIS_DOWN_THRESHOLD 0.75f

int screen_util_input_axis_pushed_left(Input* input) {
    Vec2 current, previous;
    input_axis_values(input, &current, &previous);

    return current.x <= -AXIS_DOWN_THRESHOLD && previous.x > -AXIS_DOWN_THRESHOLD;
}

int screen_util_input_axis_pushed_right(Input* input){
    Vec2 current, previous;
    input_axis_values(input, &current, &previous);

    return current.x >= AXIS_DOWN_THRESHOLD && previous.x < AXIS_DOWN_THRESHOLD;
}

int screen_util_input_axis_pushed_up(Input* input) {
    Vec2 current, previous;
    input_axis_values(input, &current, &previous);

    return current.y >= AXIS_DOWN_THRESHOLD && previous.y < AXIS_DOWN_THRESHOLD;
}

int screen_util_input_axis_pushed_down(Input* input) {
    Vec2 current, previous;
    input_axis_values(input, &current, &previous);

    return current.y <= -AXIS_DOWN_THRESHOLD && previous.y > -AXIS_DOWN_THRESHOLD;
}

int screen_util_ui_nav_down(Input* input) {
    return  input_button_is_down(input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_DOWN) ||
            input_button_is_down(input, CONTROLLER_1, CONTROLLER_BUTTON_C_DOWN) ||
            screen_util_input_axis_pushed_down(input);
}

int screen_util_ui_nav_up(Input* input) {
    return  input_button_is_down(input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_UP) ||
            input_button_is_down(input, CONTROLLER_1, CONTROLLER_BUTTON_C_UP) ||
            screen_util_input_axis_pushed_up(input);
}

int screen_util_ui_button_accept(Input* input) {
    return input_button_is_down(input, CONTROLLER_1, CONTROLLER_BUTTON_START) ||
           input_button_is_down(input, CONTROLLER_1, CONTROLLER_BUTTON_A) ||
           input_button_is_down(input, CONTROLLER_1, CONTROLLER_BUTTON_Z);
}

int screen_util_ui_nav_right(Input* input) {
    return input_button_is_down(input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_RIGHT) ||
           input_button_is_down(input, CONTROLLER_1, CONTROLLER_BUTTON_C_RIGHT) ||
           screen_util_input_axis_pushed_right(input);
}

int screen_util_ui_nav_left(Input* input) {
    return input_button_is_down(input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_LEFT) ||
           input_button_is_down(input, CONTROLLER_1, CONTROLLER_BUTTON_C_LEFT) ||
            screen_util_input_axis_pushed_left(input);
}