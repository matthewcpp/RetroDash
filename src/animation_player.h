#ifndef ANIMATION_PLAYER_H
#define ANIMATION_PLAYER_H

typedef struct {
    int beginning_frame;
    int frame_count;
    int loop;
}Animation;

typedef struct {
    Animation* animations;
    Animation* current_animation;

    float current_time;
    float total_time;
    float frame_time;
    int frame;
} AnimationPlayer;

void animation_player_init(AnimationPlayer* player, int animation_count, float frame_time);
void animation_player_uninit(AnimationPlayer* player);
void animation_player_add_animation(AnimationPlayer* player, int index, int beginning_frame, int frame_count, int loop);
void animation_player_set_current(AnimationPlayer* player, int index);
void animation_player_update(AnimationPlayer* player, float time_delta);

#endif