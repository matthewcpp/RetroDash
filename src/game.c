#include "game.h"

#include "state/playing.h"
#include "state/title.h"

#include <stdlib.h>

typedef union {
    StatePlaying* playing;
    StateTitle* title;
} State;

struct Game {
    Audio* _audio;
    Input* _input;
    Renderer* _renderer;
    GameState current_state;
    State state;
};

static void game_destroy_current_state(Game* game) {
    switch (game->current_state) {
        case GAME_STATE_TITLE:
            state_title_destroy(game->state.title);
            break;

        case GAME_STATE_PLAYING:
            state_playing_destroy(game->state.playing);
            break;

        case GAME_STATE_NONE:
            break;
    }
}

static void game_set_state(Game* game, GameState state) {
    game_destroy_current_state(game);

    switch (state) {
        case GAME_STATE_TITLE:
            game->state.title = state_title_create(game->_audio, game->_input, game->_renderer);
            break;

        case GAME_STATE_PLAYING:
            game->state.playing = state_playing_create(game->_audio, game->_renderer, game->_input, "/level01.level");
            break;

        case GAME_STATE_NONE:
            break;
    }

    game->current_state = state;
}

Game* game_create(Audio* audio, Input* input, Renderer* renderer){
    Game* game = malloc(sizeof(Game));
    game->_audio = audio;
    game->_input = input;
    game->_renderer = renderer;
    game->current_state = GAME_STATE_NONE;

    renderer_set_clear_color(game->_renderer, 10, 7, 53);
    game_set_state(game , GAME_STATE_TITLE);

    return game;
}

void game_destroy(Game* game){
    game_destroy_current_state(game);
    free(game);
}

void game_update(Game* game, float time_delta){
    GameState state_transition = GAME_STATE_NONE;

    switch (game->current_state) {
        case GAME_STATE_TITLE:
            state_title_update(game->state.title, time_delta);
            state_transition = game->state.title->transition;
            break;

        case GAME_STATE_PLAYING:
            state_playing_update(game->state.playing, time_delta);
            break;

        case GAME_STATE_NONE:
            break;
    }

    if (state_transition != GAME_STATE_NONE)
        game_set_state(game, state_transition);
}

void game_draw(Game* game){
    switch (game->current_state) {
        case GAME_STATE_TITLE:
            state_title_draw(game->state.title);
            break;

        case GAME_STATE_PLAYING:
            state_playing_draw(game->state.playing);
            break;

        case GAME_STATE_NONE:
            break;
    }
}