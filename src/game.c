#include "game.h"

#include "state/playing.h"

#include <stdlib.h>

typedef union {
    StatePlaying* playing;
} State;

struct Game {
    Renderer* _renderer;
    Input* _input;
    GameState current_state;
    State state;
};

Game* game_create(Renderer* renderer, Input* input){
    Game* game = malloc(sizeof(Game));

    game->_renderer = renderer;
    game->_input = input;
    game->current_state = GAME_STATE_PLAYING;
    game->state.playing = state_playing_create(renderer, input, "/level01.level");

    renderer_set_clear_color(renderer, 10, 7, 53);

    return game;
}

void game_destroy(Game* game){
    switch (game->current_state) {
        case GAME_STATE_PLAYING:
            state_playing_destroy(game->state.playing);
            break;
    }

    free(game);
}

void game_update(Game* game, float time_delta){
    switch (game->current_state) {
        case GAME_STATE_PLAYING:
            state_playing_update(game->state.playing, time_delta);
            break;
    }
}

void game_draw(Game* game){
    switch (game->current_state) {
        case GAME_STATE_PLAYING:
            state_playing_draw(game->state.playing);
            break;
    }
}