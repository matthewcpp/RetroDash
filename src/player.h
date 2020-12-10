#ifndef PLAYER_H
#define PLAYER_H

#include "animation_player.h"
#include "camera.h"
#include "entity.h"
#include "input.h"
#include "level.h"
#include "rect.h"
#include "renderer.h"

typedef enum {
    PLAYER_SIZE_SMALL,
    PLAYER_SIZE_MEDIUM,
    PLAYER_SIZE_LARGE
} PlayerSize;

typedef enum {
    PLAYER_STATE_IDLE,
    PLAYER_STATE_RUNNING,
    PLAYER_STATE_CHANGING_SIZE,
    PLAYER_STATE_DYING,
    PLAYER_STATE_DEAD,
    PLAYER_STATE_REACHED_GOAL
} PlayerState;

typedef enum {
    PLAYER_ANIMATION_RUN,
    PLAYER_ANIMATION_JUMP,
    PLAYER_ANIMATION_DEATH,
    PLAYER_ANIMATION_CHANGE_SIZE
} PlayerAnimation;

#define PLAYER_SPEED 6.0f

typedef struct {
    Entity entity;
    PlayerSize current_size;
    PlayerSize target_size;

    PlayerState state;

    // amount of time the player has been in their current state
    float state_time;

    // holds the time of the previous animation when the player begins changing sizes
    float prev_animation_time;

    Vec2 prev_pos;
    Vec2 velocity;

    //TODO: roll this into flags?
    int on_ground;
    int is_jumping;

    float speed_modifier;
    float distance_travelled;
    int attempt_count;
    int jump_count;
    int brick_count;
    int size_change_count;

    /** If zero, the player will not poll input for jumping or size changes. */
    int process_input;

    Sprite* _sprite;
    Level* _level;
    Renderer* _renderer;
    Input* _input;
    Camera* _camera;
    AnimationPlayer _animation;
} Player;

Player* player_create(Level* level, Renderer* renderer, Camera* camera, Input* input);
void player_destroy(Player* player);

void player_update(Player* player, float time_delta);
void player_draw(Player* player);

void player_kill(Player* player);

/**
 * Resets player to initial state.  Note they will not be running.
 */
void player_reset(Player* player);

/** Clears the players tracked stats such as jump count, brick count, distance travelled... */
void player_clear_stats(Player* player);

/**
 * Starts the player running though the level.
 * This method assumes that the player is in an inactive state.
 */
void player_start(Player* player);

/**
 * Gets the hit size for the given PlayerSize
 */
Vec2 player_get_hit_size(PlayerSize playerSize);


/**
 * Immediately sets the hit size for the player without performing any animation
 */
void player_set_hit_size(Player* player, PlayerSize playerSize);

/**
 * Attempts to start jumping.
 * If the player is changing size at this point, the jump animation will not begin until the size change is complete
 */
void player_try_jump(Player* player);

/**
 * Attempts to change the players size.
 * If the player is jumping that animation will be overridden and then restored if player still hasn't touched ground when size change is complete.
 */
void player_try_set_size(Player* player, PlayerSize size);

#endif