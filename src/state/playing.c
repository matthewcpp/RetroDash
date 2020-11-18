#include "playing.h"

#include "../rect.h"

#include <stdlib.h>

StatePlaying* state_playing_create(Renderer* renderer, Input* input, const char* level_path) {
    StatePlaying* state = malloc(sizeof(StatePlaying));

    Point screen_size;
    renderer_get_screen_size(renderer, &screen_size);

    state->camera = camera_create(screen_size.x, screen_size.y);
    state->level = level_create(renderer, state->camera);
    level_load(state->level, level_path);

    state->player = player_create(state->level, renderer, state->camera, input);
    camera_set_target(state->camera, &state->player->entity);
    camera_set_safe_margins(state->camera, -3.0f, 3.0f);
    player_start(state->player);

    return state;
}

void state_playing_destroy(StatePlaying* state){
    level_destroy(state->level);
    camera_destroy(state->camera);
    player_destroy(state->player);

    free(state);
}

void state_playing_update(StatePlaying* state, float time_delta){
    player_update(state->player, time_delta);
    level_update(state->level, time_delta);
    camera_update(state->camera);
}

void state_playing_draw(StatePlaying* state) {
    level_draw(state->level);
    player_draw(state->player);
}