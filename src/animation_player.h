#ifndef ANIMATION_PLAYER_H
#define ANIMATION_PLAYER_H

#include <stdint.h>

typedef struct {
    uint8_t frame_count;
    uint8_t* frames;
}Animation;

typedef struct {
    Animation* animations;
    Animation* current_animation;
    float frame_time;
    float current_time;
    float total_time;
    int animation_count;
    int frame;
    int loop;
} AnimationPlayer;

void animation_player_init(AnimationPlayer* player);
void animation_player_uninit(AnimationPlayer* player);
int animation_player_load(AnimationPlayer* player, const char* path);
void animation_player_set_current(AnimationPlayer* player, int index, int loop);
void animation_player_update(AnimationPlayer* player, float time_delta);
int animation_player_is_complete(AnimationPlayer* player);

#endif