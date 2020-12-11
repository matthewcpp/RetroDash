#include "playing.h"

#include <stdlib.h>

static void on_teleport_in_complete(void* user_data);
static void on_teleport_out_complete(void* user_data);
static void on_dialog_return(void* user_data);

PlayingScreen* playing_screen_create(Audio* audio, Renderer* renderer, Input* input, const char* level_path, GameSettings* settings) {
    PlayingScreen* state = malloc(sizeof(PlayingScreen));

    playing_screen_base_init(&state->base, audio, renderer, input, level_path, "/dialog/dialog_info_font", settings);

    state->transition = GAME_SCREEN_NONE;
    state->base.teleport_in_hook = on_teleport_in_complete;
    state->base.teleport_out_hook = on_teleport_out_complete;
    state->base.dialog_return_hook = on_dialog_return;
    state->base.hook_user_data = state;

    return state;
}

void playing_screen_destroy(PlayingScreen* state){
    playing_screen_base_uninit(&state->base);
    free(state);
}

void playing_screen_update(PlayingScreen* state, float time_delta){
    playing_screen_base_update(&state->base, time_delta);
}

void playing_screen_draw(PlayingScreen* state) {
    playing_screen_base_draw(&state->base);
}

void on_teleport_in_complete(void* user_data) {
    PlayingScreen* state = (PlayingScreen*)user_data;

    player_start(state->base.player);
}

void on_teleport_out_complete(void* user_data) {
    PlayingScreen* state = (PlayingScreen*)user_data;

    attempt_dialog_show(&state->base._attempt_dialog);
    audio_pause_music(state->base._audio);
}

void on_dialog_return(void* user_data) {
    PlayingScreen* state = (PlayingScreen*)user_data;

    state->transition = GAME_SCREEN_LEVEL_SELECT;
}
