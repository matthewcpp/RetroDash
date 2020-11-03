#include "player.h"

#include <math.h>
#include <stdlib.h>

#define PLAYER_SPEED 8.0f

typedef enum {
    PLAYER_ANIMATION_RUN,
    PLAYER_ANIMATION_JUMP,
    PLAYER_ANIMATION_DEATH,
    PLAYER_ANIMATION_CHANGE_SIZE
} PlayerAnimation;

typedef struct {
    int min_x, min_y;
    int max_x, max_y;
} PlayerQuery;

static Vec2 player_hit_sizes[3] = { {0.66f, 0.66f}, {1.0f, 1.0f}, {1.33, 1.33} };
static float player_jump_velocity[3] = {6.0f, 10.0f, 14.0f};
static Vec2 starting_pos = {3.0f, 3.0f};

void reset_player(Player* player);

static void player_query_init(PlayerQuery* query, Player* player);

Player* player_create(Level* level, Renderer* renderer, Camera* camera, Input* input) {
    Player* player = malloc(sizeof(Player));

    player->_level = level;
    player->_renderer = renderer;
    player->_input = input;
    player->_camera = camera;

    animation_player_init(&player->_animation);
    animation_player_load(&player->_animation, "/player.animation");
    reset_player(player);

    Sprite* sprite = renderer_load_sprite(player->_renderer, "/player");
    for (int i = 0; i < 3; i++) {
        player->_sprite = sprite;
    }

    return player;
}

void player_destroy(Player* player) {
    animation_player_uninit(&player->_animation);
    renderer_destroy_sprite(player->_renderer, player->_sprite);
    free(player);
}

/**
 * Checks to see if the player is standing on safe ground.
 * In the event they are, it will update the position such that they are standing on the top of the tile.
 * Note that gravity is always applied causing the player to "sink" which is the reason for the adjustment.
 */
static void check_floor(Player* player, PlayerQuery* query) {
    player->on_ground = 0;

    for (int x = query->min_x; x <= query->max_x; x++) {
        Tile* tile = level_get_tile(player->_level, x, query->min_y);

        if (tile == NULL) continue;

        switch (tile->type) {
            // if the player has fallen into a solid tile, update their position so that they are standing on it
            case TILE_TYPE_SOLID:
                player->entity.position.y = ceil(player->entity.position.y);
                player->on_ground = 1;
                player->velocity.y = 0.0f;

                // need to update the query
                query->min_y = (int)player->entity.position.y;
                query->max_y = (int)floor(player->entity.position.y + player_hit_sizes[player->current_size].y);

                if (player->is_jumping) {
                    player->is_jumping = 0;
                    animation_player_set_current(&player->_animation, PLAYER_ANIMATION_RUN, 1);
                }
                return;

            case TILE_TYPE_KILL:
                player_kill(player);
                return;
        }
    }
}

/**
 * Check to see if the player has collided with any tile besides a safe ground tile.
 */
static void check_collisions(Player* player, PlayerQuery* query) {
    // check to see if the player has collided with the world
    for (int y = query->min_y; y <= query->max_y; y++) {

        Tile* tile = level_get_tile(player->_level, query->max_x, y);

        if (tile == NULL) continue;
        switch (tile->type) {
            case TILE_TYPE_SOLID:
            case TILE_TYPE_KILL:
                player_kill(player);
                return;

            case TILE_TYPE_TUNNEL:
                if (player->current_size != PLAYER_SIZE_SMALL) {
                    player_kill(player);
                    return;
                }
                break;

            case TILE_TYPE_BRICK:
                if (player->current_size == PLAYER_SIZE_LARGE) {
                    level_set_tile(player->_level, query->max_x, y, TILE_EMPTY);
                    break;
                }
                else {
                    player_kill(player);
                    return;
                }
        }
    }
}

static void try_jump(Player* player) {
    if (!player->on_ground)
        return;

    player->velocity.y = player_jump_velocity[player->target_size];
    player->on_ground = 0;
    player->is_jumping = 1;
    animation_player_set_current(&player->_animation, PLAYER_ANIMATION_JUMP, 0);
}

static void try_set_size(Player* player, PlayerSize size) {
    if (player->state != PLAYER_STATE_RUNNING || size == player->current_size)
        return;

    player->state = PLAYER_STATE_CHANGING_SIZE;
    player->target_size = size;
    player->state_time = 0.0f;
    animation_player_set_current(&player->_animation, PLAYER_ANIMATION_CHANGE_SIZE, 0);
}

void player_update_movement(Player* player, float time_delta) {
    player->prev_pos = player->entity.position;

    // step vertical
    player->velocity.y -= player->_level->gravity * time_delta;
    player->entity.position.y += player->velocity.y * time_delta;

    PlayerQuery query;
    player_query_init(&query, player);

    check_floor(player, &query);

    // step horizontal
    player->entity.position.x += player->velocity.x * time_delta;
    check_collisions(player, &query);

    if (input_button_is_down(player->_input, CONTROLLER_1, CONTROLLER_BUTTON_A)) {
        try_jump(player);
    }
    if (input_button_is_down(player->_input, CONTROLLER_1, CONTROLLER_BUTTON_C_LEFT)) {
        try_set_size(player, PLAYER_SIZE_SMALL);
    }
    if (input_button_is_down(player->_input, CONTROLLER_1, CONTROLLER_BUTTON_C_UP)) {
        try_set_size(player, PLAYER_SIZE_MEDIUM);
    }
    if (input_button_is_down(player->_input, CONTROLLER_1, CONTROLLER_BUTTON_C_RIGHT)) {
        try_set_size(player, PLAYER_SIZE_LARGE);
    }

    if (player->entity.position.x >= player->_level->goal_dist)
        player->velocity.x = 0.0f;

    animation_player_update(&player->_animation, time_delta);
}

void player_update_dying(Player* player, float time_delta) {
    player->state_time += time_delta;
    animation_player_update(&player->_animation, time_delta);

    if (player->state_time >= 1.0f) {
        player_start(player);
    }
}

void player_update_changing_size(Player* player, float time_delta) {
    player->state_time += time_delta;
    animation_player_update(&player->_animation, time_delta);
    player_update_movement(player, time_delta);

    // If player has reached target size, return status to running
    if (animation_player_is_complete(&player->_animation)) {
        player->state = PLAYER_STATE_RUNNING;
        animation_player_set_current(&player->_animation, PLAYER_ANIMATION_RUN, 1);
        player->current_size = player->target_size;
        player->entity.size = player_hit_sizes[player->current_size];
        return;
    }

    // LERP player size towards the target size
    float t = player->state_time / player->_animation.total_time;
    player->entity.size.x = player_hit_sizes[player->current_size].x + t * (player_hit_sizes[player->target_size].x - player_hit_sizes[player->current_size].x);
    player->entity.size.y = player_hit_sizes[player->current_size].y + t * (player_hit_sizes[player->target_size].y - player_hit_sizes[player->current_size].y);
}

void player_update(Player* player, float time_delta) {
    switch (player->state) {
        case PLAYER_STATE_RUNNING:
            player_update_movement(player, time_delta);
            break;

        case PLAYER_STATE_DYING:
            player_update_dying(player, time_delta);
            break;

        case PLAYER_STATE_CHANGING_SIZE:
            player_update_changing_size(player, time_delta);
            break;

        default:
            break;
    }
}

void player_draw(Player* player) {
    Point draw_pos;
    camera_world_pos_to_screen_pos(player->_camera, &player->entity.position, &draw_pos);

    draw_pos.x -= (int)((sprite_horizontal_frame_size(player->_sprite) *  player_hit_sizes[player->current_size].x) / 2.0f);
    draw_pos.y -= (int)((sprite_vertical_frame_size(player->_sprite) * player_hit_sizes[player->current_size].y));

    renderer_draw_scaled_sprite(player->_renderer, player->_sprite,
                                draw_pos.x, draw_pos.y,
                                player->entity.size.x, player->entity.size.y,
                                player->_animation.frame);
}

void player_start(Player* player) {
    reset_player(player);

    player->state = PLAYER_STATE_RUNNING;
    player->velocity.x = PLAYER_SPEED;
    animation_player_set_current(&player->_animation, PLAYER_ANIMATION_RUN, 1);
}

void player_kill(Player* player) {
    player->state_time = 0.0f;
    player->state = PLAYER_STATE_DYING;
    animation_player_set_current(&player->_animation, PLAYER_ANIMATION_DEATH, 0);
}

void reset_player(Player* player) {
    player->current_size = PLAYER_SIZE_MEDIUM;
    player->target_size = PLAYER_SIZE_MEDIUM;
    player->entity.size = player_hit_sizes[PLAYER_SIZE_MEDIUM];

    player->state = PLAYER_STATE_INACTIVE;
    player->state_time = 0.0f;

    player->entity.position.x = starting_pos.x;
    player->entity.position.y = starting_pos.y;
    player->prev_pos = player->entity.position;
    player->velocity.x = 0.0f;
    player->velocity.y = 0.0f;

    player->on_ground = 1;
    player->is_jumping = 0;
}

// TODO: This should take previous position into account to prevent "falling through tiles"
void player_query_init(PlayerQuery* query, Player* player) {
    query->max_x = (int)floor(player->entity.position.x + player_hit_sizes[player->current_size].x / 2.0f); // right
    if (query->max_x >= player->_level->width) query->max_x = player->_level->width - 1;

    query->max_y = (int)floor(player->entity.position.y + player_hit_sizes[player->current_size].y); // top
    if (query->max_y >= player->_level->height) query->max_y = player->_level->height - 1;

    query->min_y = (int)floor(player->entity.position.y); // bottom
    if (query->min_y < 0) query->min_y = 0;

    query->min_x = (int)floor(player->entity.position.x - player_hit_sizes[player->current_size].x / 2.0f); // left
    if (query->min_x < 0) query->min_x = 0;
}
