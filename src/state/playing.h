#ifndef PLAYING_H
#define PLAYING_H

#include "../audio.h"
#include "../camera.h"
#include "../level.h"
#include "../player.h"
#include "../teleport.h"

#include "playing_base.h"
#include "states.h"

typedef struct {
    StatePlayingBase base;
    GameState transition;
} StatePlaying;

StatePlaying* state_playing_create(Audio* audio, Renderer* renderer, Input* input, const char* level_path);
void state_playing_destroy(StatePlaying* state);

void state_playing_update(StatePlaying* state, float time_delta);
void state_playing_draw(StatePlaying* state);

#endif