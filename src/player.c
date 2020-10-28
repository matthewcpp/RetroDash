#include "player.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

static Vec2 player_hit_sizes[3] = { {0.66f, 0.66f}, {1.0f, 1.0f}, {1.33, 1.33} };
static float player_jump_velocity[3] = {6.0f, 8.0f, 10.0f};
static Vec2 starting_pos = {0.5f, 2.0f};

void reset_player(Player* player);

Player* player_create(Level* level, Renderer* renderer, Camera* camera, Input* input) {
    Player* player = malloc(sizeof(Player));

    player->_level = level;
    player->_renderer = renderer;
    player->_input = input;
    player->_camera = camera;

    reset_player(player);

    Sprite* sprite = renderer_load_sprite(player->_renderer, "/player");
    for (int i = 0; i < 3; i++) {
        player->_sprites[i] = sprite;
    }

    return player;
}

void player_update(Player* player, float time_delta) {
    //player->velocity.x = 6;

    if (input_button_is_down(player->_input, CONTROLLER_1, CONTROLLER_BUTTON_L)) {
        reset_player(player);
    }

    int query_min_x = (int)floor(player->position.x - player_hit_sizes[player->size].x / 2.0f); // left
    int query_max_x = (int)floor(player->position.x + player_hit_sizes[player->size].x / 2.0f); // right
    int query_max_y = (int)floor(player->position.y + player_hit_sizes[player->size].y); // top
    int query_min_y = (int)floor(player->position.y); // bottom

    // check to see if the player is standing on the ground
    player->on_ground = 0;
    for (int x = query_min_x; x < query_max_x; x++) {
        Tile* tile = level_get_tile(player->_level, x, query_min_y);

        if (tile == NULL) continue;

        if ((tile->bits & TILE_BIT_SOLID) == TILE_BIT_SOLID && !player->on_ground) {
            player->position.y = (float) query_max_y;
            player->on_ground = 1;
        }
    }
/*
    // check to see if the player has collided with the world
    for (int y = query_min_y; y < query_max_y; y++) {
        Tile* tile = level_get_tile(player->_level, query_max_x, y);

        if (tile == NULL) continue;

        if ((tile->bits & TILE_BIT_SOLID) == TILE_BIT_SOLID) {
            reset_player(player);
            return;
        }
    }
*/
    if (player->on_ground) {
        player->velocity.y = 0;

        if (input_button_is_down(player->_input, CONTROLLER_1, CONTROLLER_BUTTON_A)) {
            player->velocity.y = player_jump_velocity[player->size];
            player->on_ground = 0;
        }
    }

    if (!player->on_ground) {
        player->velocity.y -= player->_level->gravity * time_delta;
    }

    player->acceleration.x = player->velocity.x * time_delta;
    player->acceleration.y = player->velocity.y * time_delta;

    player->position.x += player->acceleration.x;
    player->position.y += player->acceleration.y;
}

void player_draw(Player* player) {
    Point draw_pos;
    camera_world_pos_to_screen_pos(player->_camera, &player->position, &draw_pos);


    Sprite* sprite = player->_sprites[player->size];
    draw_pos.x -= (sprite_width(sprite) / 2);
    draw_pos.y -= (sprite_height(sprite));

    renderer_draw_sprite(player->_renderer, sprite, draw_pos.x, draw_pos.y, 0);
}

void reset_player(Player* player) {
    puts("reset player");
    player->size = PLAYER_SIZE_MEDIUM;
    player->position.x = starting_pos.x;
    player->position.y = starting_pos.y;
    player->on_ground = 1;

    player->acceleration.x = 0.0f;
    player->acceleration.y = 0.0f;
    player->velocity.x = 0.0f;
    player->velocity.y = 0.0f;
}
