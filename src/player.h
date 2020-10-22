#ifndef PLAYER_H
#define PLAYER_H

#include "input.h"
#include "level.h"
#include "renderer.h"

typedef struct {
	Level* _level;
    Input* _input;
    Renderer* _renderer;

    float pos_x, pos_y;
    float jump_velocity;
    float frame_time;
    int sprite_frame;
    int on_ground;
    float velocity;
} Player;

Player* player_create(Level* level, Input* input, Renderer* renderer);
void player_update(Player* player, float time);
void player_draw(Player* player);

#endif