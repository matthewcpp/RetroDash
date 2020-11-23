#include "playing.h"

#include <stdlib.h>

StatePlaying* state_playing_create(Audio* audio, Renderer* renderer, Input* input, const char* level_path) {
    StatePlaying* state = malloc(sizeof(StatePlaying));

    state->_audio = audio;
    state->_input = input;
    state->_renderer = renderer;
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
    state->_just_loaded = 1;

    state->_title_font = renderer_load_font(renderer, "/dialog_title_font", "/dialog_title_font.font");
    state->_info_font = renderer_load_font(renderer, "/dialog_info_font", "/dialog_info_font.font");

    attempt_dialog_init(&state->_attempt_dialog, input, renderer, state->player, state->_title_font, state->_info_font);
    pause_dialog_init(&state->_pause_dialog, input, renderer, state->_title_font, state->_info_font);

    return state;
}

void state_playing_destroy(StatePlaying* state){
    level_destroy(state->level);
    camera_destroy(state->camera);
    player_destroy(state->player);

    renderer_destroy_font(state->_renderer, state->_title_font);
    renderer_destroy_font(state->_renderer, state->_info_font);

    free(state);
}

/**
 * Resets the player, level, and audio for another attempt.  This method assumes the player is in a DEAD state.
 */

static void reset_scene(StatePlaying* state) {
    state->_attempt_dialog.base.shown = 0;
    level_reset(state->player->_level);
    player_start(state->player);
    audio_restart_music(state->_audio);
}

static void update_player_active(StatePlaying* state, float time_delta) {
    if (input_button_is_down(state->_input, CONTROLLER_1, CONTROLLER_BUTTON_START)) {
        audio_pause_music(state->_audio);
        state->_paused = 1;
    }

    player_update(state->player, time_delta);
    level_update(state->level, time_delta);
    camera_update(state->camera);
}

static void update_state_paused(StatePlaying* state, float time_delta) {
    if (!state->_pause_dialog.base.shown) {
        pause_dialog_show(&state->_pause_dialog);
    }
    else {
        pause_dialog_update(&state->_pause_dialog);
    }

    if (state->_pause_dialog.base.action == DIALOG_ACTION_RETRY) {
        state->_paused = 0;
        audio_resume_music(state->_audio);
        pause_dialog_hide(&state->_pause_dialog);
    }
    else if (state->_pause_dialog.base.action == DIALOG_ACTION_RETURN){
        state->transition = GAME_STATE_LEVEL_SELECT;
    }
}

void state_playing_update(StatePlaying* state, float time_delta){
    if (state->_just_loaded == 1) {
        player_start(state->player);
        audio_play_music(state->_audio, state->level->music);
        state->_just_loaded = 0;
        return;
    }

    if (state->_paused) {
        update_state_paused(state, time_delta);
    }
    else if (state->player->state == PLAYER_STATE_DEAD || state->player->state == PLAYER_STATE_REACHED_GOAL) {
        if (!state->_attempt_dialog.base.shown)
            attempt_dialog_show(&state->_attempt_dialog);

        attempt_dialog_update(&state->_attempt_dialog);

        if (state->_attempt_dialog.base.action == DIALOG_ACTION_RETURN){
            attempt_dialog_hide(&state->_attempt_dialog);
            state->transition = GAME_STATE_LEVEL_SELECT;
        }
        else if (state->_attempt_dialog.base.action == DIALOG_ACTION_RETRY) {
            attempt_dialog_hide(&state->_attempt_dialog);
            reset_scene(state);
        }
    }
    else {
        update_player_active(state, time_delta);
    }
}

void state_playing_draw(StatePlaying* state) {
    level_draw(state->level);
    player_draw(state->player);

    if (state->_attempt_dialog.base.shown) {
        attempt_dialog_draw(&state->_attempt_dialog);
    }

    if (state->_pause_dialog.base.shown) {
        pause_dialog_draw(&state->_pause_dialog);
    }
}