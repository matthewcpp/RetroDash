#include "animation_player.h"
#include "filesystem.h"

#include <math.h>
#include <stdlib.h>

#define FRAME_TIME 0.1f

void animation_player_init(AnimationPlayer* player) {
    player->animations = NULL;
    player->current_animation = NULL;
    player->current_time = 0.0f;
    player->frame = 0;
    player->loop = 0;
}

void animation_player_uninit(AnimationPlayer* player) {
    for (int i = 0; i < player->animation_count; i++) {
        Animation* animation = player->animations + i;
        free(animation->frames);
    }

    free(player->animations);
}

int animation_player_load(AnimationPlayer* player, const char* path) {
    int animation_file = filesystem_open(path);

    if (animation_file < 0) return 0;

    uint8_t animation_count;
    filesystem_read(&animation_count, sizeof(uint8_t), 1, animation_file);
    player->animation_count = animation_count;
    player->animations = calloc(animation_count, sizeof(Animation));

    for (uint8_t i = 0; i < animation_count; i++) {
        Animation* animation = player->animations + i;
        filesystem_read(&animation->frame_count, sizeof(uint8_t), 1, animation_file);
        animation->frames = malloc(animation->frame_count);
        filesystem_read(animation->frames, sizeof(uint8_t), animation->frame_count, animation_file);
    }

    filesystem_close(animation_file);
    return 1;
}

void animation_player_set_current(AnimationPlayer* player, int index, int loop) {
    player->current_animation = player->animations + index;
    player->total_time = player->current_animation->frame_count * FRAME_TIME;
    player->current_time = 0.0f;
    player->frame = player->current_animation->frames[0];
    player->loop = loop;
}

void animation_player_update(AnimationPlayer* player, float time_delta) {
    player->current_time += time_delta;

    if (player->current_time >= player->total_time) {
        if (player->loop)
            player->current_time -= player->total_time;
        else
            player->current_time = player->total_time;
    }

    int frame_index = (int)floor(player->current_time / FRAME_TIME);

    // prevent us from going beyond the frame range when current_time == total_time and we are not looping
    if (frame_index >= player->current_animation->frame_count)
        frame_index = player->current_animation->frame_count - 1;

    player->frame = player->current_animation->frames[frame_index];
}

int animation_player_is_complete(AnimationPlayer* player) {
    return player->current_time == player->total_time && !player->loop;
}