#ifndef PLAYING_H
#define PLAYING_H

#include "../audio.h"
#include "../camera.h"
#include "../level.h"
#include "../player.h"
#include "states.h"

typedef struct {
    Camera* camera;
    Level* level;
    Player* player;

    Input* _input;
    Audio* _audio;
    int _paused;
    GameState transition;
} StatePlaying;

StatePlaying* state_playing_create(Audio* audio, Renderer* renderer, Input* input, const char* level_path);
void state_playing_destroy(StatePlaying* state);

void state_playing_update(StatePlaying* state, float time_delta);
void state_playing_draw(StatePlaying* state);

#endif