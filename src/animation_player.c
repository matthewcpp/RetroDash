#include "animation_player.h"

#include <math.h>
#include <stdlib.h>

void animation_player_init(AnimationPlayer* player, int animation_count, float frame_time) {
    player->animations = calloc(animation_count, sizeof(Animation));
    player->current_animation = NULL;
    player->current_time = 0.0f;
    player->frame_time = frame_time;
    player->frame = 0;
}

void animation_player_uninit(AnimationPlayer* player) {
    free(player->animations);
}

void animation_player_add_animation(AnimationPlayer* player, int handle, int beginning_frame, int frame_count, int loop) {
    Animation* animation = player->animations + handle;
    animation->beginning_frame = beginning_frame;
    animation->frame_count = frame_count;
    animation->loop = loop;
}

void animation_player_set_current(AnimationPlayer* player, int index) {
    player->current_animation = player->animations + index;
    player->total_time = player->current_animation->frame_count * player->frame_time;
    player->current_time = 0.0f;
    player->frame = 0;
}

void animation_player_update(AnimationPlayer* player, float time_delta) {
    player->current_time += time_delta;

    if (player->current_time >= player->total_time) {
        if (player->current_animation->loop) {
            player->current_time -= player->total_time;
        }
        else
            player->current_time = player->total_time;
    }

    player->frame = (int)floor(player->current_time / player->frame_time);

    // prevent us from going beyond the frame range when current_time == total_time and we are not looping
    if (player->frame >= player->current_animation->frame_count)
        player->frame = player->current_animation->frame_count - 1;
}