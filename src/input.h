#ifndef INPUT_H
#define INPUT_H

typedef struct Input Input;


typedef enum {
    CONTROLLER_BUTTON_START,
    CONTROLLER_BUTTON_A,
    CONTROLLER_BUTTON_B,
    CONTROLLER_BUTTON_Z,
    CONTROLLER_BUTTON_L,
    CONTROLLER_BUTTON_R,

    CONTROLLER_BUTTON_C_UP,
    CONTROLLER_BUTTON_C_DOWN,
    CONTROLLER_BUTTON_C_LEFT,
    CONTROLLER_BUTTON_C_RIGHT,

    CONTROLLER_BUTTON_DPAD_UP,
    CONTROLLER_BUTTON_DPAD_DOWN,
    CONTROLLER_BUTTON_DPAD_LEFT,
    CONTROLLER_BUTTON_DPAD_RIGHT,
    CONTROLLER_BUTTON_COUNT
} ControllerButton;

typedef enum {
    CONTROLLER_1 = 0,
    CONTROLLER_2 = 1,
    CONTROLLER_3 = 2,
    CONTROLLER_4 = 3
} ControllerIndex;

int input_button_is_down(Input* input, ControllerIndex controller, ControllerButton button);
int input_button_is_up(Input* input, ControllerIndex controller, ControllerButton button);
int input_button_is_held(Input* input, ControllerIndex controller, ControllerButton button);

#endif