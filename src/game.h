#ifndef GAME_H
#define GAME_H

#include "audio.h"
#include "input.h"
#include "renderer.h"
#include "state/states.h"

typedef struct Game Game;

Game* game_create(Audio* audio, Input* input, Renderer* renderer);
void game_destroy(Game* game);

void game_update(Game* game, float time_delta);
void game_draw(Game* game);

#endif