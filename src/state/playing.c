#include "playing.h"

#include <stdlib.h>

StatePlaying* state_playing_create(Audio* audio, Renderer* renderer, Input* input, const char* level_path) {
    StatePlaying* state = malloc(sizeof(StatePlaying));

    state->_audio = audio;
    state->_input = input;
    state->_paused = 0;
    state->transition = GAME_STATE_NONE;

    Point screen_size;
    renderer_get_screen_size(renderer, &screen_size);

    state->camera = camera_create(screen_size.x, screen_size.y);
    state->level = level_create(audio, renderer, state->camera);
    level_load(state->level, level_path);

    state->player = player_create(state->level, renderer, state->camera, input);
    camera_set_target(state->camera, &state->player->entity);
    camera_set_safe_margins(state->camera, -3.0f, 3.0f);
    player_start(state->player);
    audio_play_music(state->_audio, state->level->music);

    return state;
}

void state_playing_destroy(StatePlaying* state){
    level_destroy(state->level);
    camera_destroy(state->camera);
    player_destroy(state->player);

    free(state);
}

static void toggle_pause(StatePlaying* state) {
    if (state->_paused) {
        state->_paused = 0;
        audio_resume_music(state->_audio);
    }
    else {
        audio_pause_music(state->_audio);
        state->_paused = 1;
    }
}

void state_playing_update(StatePlaying* state, float time_delta){
    if (input_button_is_down(state->_input, CONTROLLER_1, CONTROLLER_BUTTON_START))
        toggle_pause(state);

    if (state->_paused)
        return;

    player_update(state->player, time_delta);
    level_update(state->level, time_delta);
    camera_update(state->camera);

    if (state->player->state == PLAYER_STATE_REACHED_GOAL) {
        state->transition = GAME_STATE_LEVEL_SELECT;
    }
}

void state_playing_draw(StatePlaying* state) {
    level_draw(state->level);
    player_draw(state->player);
}