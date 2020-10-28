#ifndef PLAYER_H
#define PLAYER_H

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
    Vec2 acceleration;
    Vec2 velocity;
    int on_ground;

    Sprite*_sprites[3];
    Level* _level;
    Renderer* _renderer;
    Input* _input;
    Camera* _camera;
} Player;

Player* player_create(Level* level, Renderer* renderer, Camera* camera, Input* input);
void player_update(Player* player, float time_delta);
void player_draw(Player* player);

#endif