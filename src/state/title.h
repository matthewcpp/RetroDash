#ifndef TITLE_H
#define TITLE_H

#include "../input.h"
#include "../renderer.h"
#include "states.h"

typedef struct {
    Input* _input;
    Renderer* _renderer;
    GameState transition;
    Sprite* _title_sprite;
} StateTitle;

StateTitle* state_title_create(Renderer* renderer, Input* input);
void state_title_destroy(StateTitle* state);

void state_title_update(StateTitle* state, float time_delta);
void state_title_draw(StateTitle* state);

#endif