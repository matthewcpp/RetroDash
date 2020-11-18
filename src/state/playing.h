#ifndef PLAYING_H
#define PLAYING_H

#include "../player.h"
#include "../camera.h"
#include "../level.h"

typedef struct {
    Camera* camera;
    Level* level;
    Player* player;

    Input* _input;
    int _paused;
} StatePlaying;

StatePlaying* state_playing_create(Renderer* renderer, Input* input, const char* level_path);
void state_playing_destroy(StatePlaying* state);

void state_playing_update(StatePlaying* state, float time_delta);
void state_playing_draw(StatePlaying* state);

#endif