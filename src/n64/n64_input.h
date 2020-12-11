#ifndef N64_INPUT_H
#define N64_INPUT_H

#include "../input.h"
#include "../rect.h"

#include <libdragon.h>

struct Input {
    struct controller_data current_state;
    struct controller_data previous_state;

    Vec2 axis_current;
    Vec2 axis_prev;
};

Input* n64_input_create();
void n64_input_update(Input* input);


#endif