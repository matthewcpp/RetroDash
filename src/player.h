#ifndef PLAYER_H
#define PLAYER_H

#include "animation_player.h"
#include "camera.h"
#include "entity.h"
#include "input.h"
#include "level.h"
#include "rect.h"
#include "renderer.h"

typedef enum {
    PLAYER_SIZE_SMALL,
    PLAYER_SIZE_MEDIUM,
    PLAYER_SIZE_LARGE
} PlayerSize;

typedef enum {
    PLAYER_STATE_INACTIVE,
    PLAYER_STATE_RUNNING,
    PLAYER_STATE_CHANGING_SIZE,
    PLAYER_STATE_DYING
} PlayerState;

typedef struct {
    Entity entity;
    PlayerSize current_size;
    PlayerSize target_size;

    PlayerState state;

    // amount of time the player has been in thier current state
    float state_time;

    // holds the time of the previous animation when the player begins changing sizes
    float prev_animation_time;

    Vec2 prev_pos;
    Vec2 velocity;

    //TODO: roll this into flags?
    int on_ground;
    int is_jumping;


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