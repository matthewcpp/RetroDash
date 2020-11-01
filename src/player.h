#ifndef PLAYER_H
#define PLAYER_H

#include "animation_player.h"
#include "camera.h"
#include "input.h"
#include "level.h"
#include "rect.h"
#include "renderer.h"

typedef enum {
    PLAYER_SIZE_SMALL,
    PLAYER_SIZE_MEDIUM,
    PLAYER_SIZE_LARGE
} PlayerSize;

typedef struct {
    PlayerSize size;

    Vec2 position;
    Vec2 velocity;

    Box bounding_box;
    int on_ground;

    Vec2 prev_pos;

    Sprite* _sprite;
    Level* _level;
    Renderer* _renderer;
    Input* _input;
    Camera* _camera;
    AnimationPlayer _animation;
} Player;

Player* player_create(Level* level, Renderer* renderer, Camera* camera, Input* input);
void player_destroy(Player* player);

void player_update(Player* player, float time_delta);
void player_draw(Player* player);

void player_kill(Player* player);
void player_start(Player* player);

#endif