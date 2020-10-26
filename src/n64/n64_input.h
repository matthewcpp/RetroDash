#ifndef N64_INPUT_H
#define N64_INPUT_H

#include "../input.h"

#include <libdragon.h>

struct Input {
    struct controller_data buttons_down;
    struct controller_data buttons_up;
    struct controller_data buttons_held;
};

Input* n64_input_create();
void n64_input_update(Input* input);


#endif