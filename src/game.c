#include "game.h"

#include "screens/level_select.h"
#include "screens/playing.h"
#include "screens/settings.h"
#include "screens/title.h"
#include "screens/tutorial.h"

#include <stdlib.h>
#include <string.h>

typedef union {
    PlayingScreen* playing;
    TitleScreen* title;
    LevelSelectScreen* level_select;
    TutorialScreen* tutorial;
    SettingsScreen * settings;
} State;

struct Game {
    Audio* _audio;
    Input* _input;
    Renderer* _renderer;
    GameState current_state;
    State state;
    Settings settings;
};

static void game_destroy_current_state(Game* game) {
    switch (game->current_state) {
        case GAME_STATE_TITLE:
            title_screen_destroy(game->state.title);
            break;

        case GAME_STATE_PLAYING:
            playing_screen_destroy(game->state.playing);
            break;

        case GAME_STATE_LEVEL_SELECT:
            level_select_screen_destroy(game->state.level_select);
            break;

        case GAME_STATE_TUTORIAL:
            tutorial_screen_destroy(game->state.tutorial);
            break;

        case GAME_STATE_SETTINGS:
            settings_screen_destroy(game->state.settings);
            break;

        case GAME_STATE_NONE:
            break;
    }
}

static void game_set_state(Game* game, GameState state) {
    switch (state) {
        case GAME_STATE_TITLE:
            game_destroy_current_state(game);
            game->state.title = title_screen_create(game->_audio, game->_input, game->_renderer);
            break;

        case GAME_STATE_PLAYING: {
            char level_path[32];
            strcpy(level_path, level_select_screen_get_selected_path(game->state.level_select));
            game_destroy_current_state(game);
            game->state.playing = playing_screen_create(game->_audio, game->_renderer, game->_input, level_path,
                                                        &game->settings);
            break;
        }

        case GAME_STATE_LEVEL_SELECT:
            game_destroy_current_state(game);
            game->state.level_select = level_select_screen_create(game->_audio, game->_input, game->_renderer);
            break;

        case GAME_STATE_TUTORIAL:
            game_destroy_current_state(game);
            game->state.tutorial = tutorial_screen_create(game->_audio, game->_input, game->_renderer);
            break;

        case GAME_STATE_SETTINGS:
            game_destroy_current_state(game);
            game->state.settings = settings_screen_create(game->_renderer, game->_input, &game->settings);
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

    settings_init(&game->settings);

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
            title_screen_update(game->state.title, time_delta);
            state_transition = game->state.title->transition;
            break;

        case GAME_STATE_PLAYING:
            playing_screen_update(game->state.playing, time_delta);
            state_transition = game->state.playing->transition;
            break;

        case GAME_STATE_LEVEL_SELECT:
            level_select_screen_update(game->state.level_select, time_delta);
            state_transition = game->state.level_select->transition;
            break;

        case GAME_STATE_TUTORIAL:
            tutorial_screen_update(game->state.tutorial, time_delta);
            state_transition = game->state.tutorial->transition;
            break;

        case GAME_STATE_SETTINGS:
            settings_screen_update(game->state.settings, time_delta);
            state_transition = game->state.settings->transition;
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
            title_screen_draw(game->state.title);
            break;

        case GAME_STATE_PLAYING:
            playing_screen_draw(game->state.playing);
            break;

        case GAME_STATE_LEVEL_SELECT:
            level_select_screen_draw(game->state.level_select);
            break;

        case GAME_STATE_TUTORIAL:
            tutorial_screen_draw(game->state.tutorial);
            break;

        case GAME_STATE_SETTINGS:
            settings_screen_draw(game->state.settings);
            break;

        case GAME_STATE_NONE:
            break;
    }
}