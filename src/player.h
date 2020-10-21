#ifndef PLAYER_H
#define PLAYER_H

#include "input.h"
#include "renderer.h"

typedef struct {
    Input* _input;
    Renderer* _renderer;

    float _prev_x, _prev_y;
    float pos_x, pos_y;
    float frame_time;
    int sprite_frame;
} Player;

Player* player_create(Input* input, Renderer* renderer);
void player_update(Player* player, float time);
void player_draw(Player* player);

#endif