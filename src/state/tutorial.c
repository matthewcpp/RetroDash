#include "tutorial.h"

#include <stdlib.h>

static void on_teleport_in_complete(void* user_data);
static void on_teleport_out_complete(void* user_data);
static void on_dialog_return(void* user_data);

StateTutorial* state_tutorial_create(Audio* audio, Input* input, Renderer* renderer) {
    StateTutorial* tutorial = malloc(sizeof(StateTutorial));

    state_playing_base_init(&tutorial->base, audio, renderer, input, "/tutorial.level");

    tutorial->transition = GAME_STATE_NONE;
    tutorial->base.teleport_in_hook = on_teleport_in_complete;
    tutorial->base.teleport_out_hook = on_teleport_out_complete;
    tutorial->base.dialog_return_hook = on_dialog_return;
    tutorial->base.hook_user_data = tutorial;

    return tutorial;
}

void state_tutorial_destroy(StateTutorial* tutorial){
    state_playing_base_uninit(&tutorial->base);
    free(tutorial);
}

void state_tutorial_update(StateTutorial* tutorial, float time_delta){
    state_playing_base_update(&tutorial->base, time_delta);
}

void state_tutorial_draw(StateTutorial* tutorial) {
    state_playing_base_draw(&tutorial->base);
}

void on_teleport_in_complete(void* user_data) {
    StateTutorial* tutorial = (StateTutorial*)user_data;

    player_start(tutorial->base.player);
}

void on_teleport_out_complete(void* user_data) {
    StateTutorial* tutorial = (StateTutorial*)user_data;

    attempt_dialog_show(&tutorial->base._attempt_dialog);
    audio_pause_music(tutorial->base._audio);
}

void on_dialog_return(void* user_data) {
    StateTutorial* state = (StateTutorial*)user_data;

    state->transition = GAME_STATE_TITLE;
}
