#ifndef GAME_H
#define GAME_H

#include "renderer.h"
#include "input.h"

typedef enum {
    GAME_STATE_PLAYING
} GameState;

typedef struct Game Game;

Game* game_create(Renderer* renderer, Input* input);
void game_destroy(Game* game);

void game_update(Game* game, float time_delta);
void game_draw(Game* game);

#endif