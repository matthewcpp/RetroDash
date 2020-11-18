#ifndef GAME_H
#define GAME_H

#include "input.h"
#include "renderer.h"
#include "state/states.h"

typedef struct Game Game;

Game* game_create(Renderer* renderer, Input* input);
void game_destroy(Game* game);

void game_update(Game* game, float time_delta);
void game_draw(Game* game);

#endif