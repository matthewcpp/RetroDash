#include "playing.h"

#include <stdlib.h>

static void on_teleport_in_complete(void* user_data);
static void on_teleport_out_complete(void* user_data);
static void on_dialog_return(void* user_data);

StatePlaying* state_playing_create(Audio* audio, Renderer* renderer, Input* input, const char* level_path) {
    StatePlaying* state = malloc(sizeof(StatePlaying));

    state_playing_base_init(&state->base, audio, renderer, input, level_path, "/dialog/dialog_info_font");
    state->transition = GAME_STATE_NONE;
    state->base.teleport_in_hook = on_teleport_in_complete;
    state->base.teleport_out_hook = on_teleport_out_complete;
    state->base.dialog_return_hook = on_dialog_return;
    state->base.hook_user_data = state;

    return state;
}

void state_playing_destroy(StatePlaying* state){
    state_playing_base_uninit(&state->base);
    free(state);
}

void state_playing_update(StatePlaying* state, float time_delta){
    state_playing_base_update(&state->base, time_delta);
}

void state_playing_draw(StatePlaying* state) {
    state_playing_base_draw(&state->base);
}

void on_teleport_in_complete(void* user_data) {
    StatePlaying* state = (StatePlaying*)user_data;

    player_start(state->base.player);
}

void on_teleport_out_complete(void* user_data) {
    StatePlaying* state = (StatePlaying*)user_data;

    attempt_dialog_show(&state->base._attempt_dialog);
    audio_pause_music(state->base._audio);
}

void on_dialog_return(void* user_data) {
    StatePlaying* state = (StatePlaying*)user_data;

    state->transition = GAME_STATE_LEVEL_SELECT;
}
