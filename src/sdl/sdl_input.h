#ifndef SDL_INPUT_H
#define SDL_INPUT_H

#include "../input.h"

#include <SDL.h>

Input* sdl_input_create();
void sdl_input_update(Input* input);



#endif