#include "player.h"

#include <math.h>
#include <stdlib.h>

static Vec2 player_hit_sizes[3] = { {0.66f, 0.66f}, {1.0f, 1.0f}, {1.33, 1.33} };

Player* player_create(Level* level, Renderer* renderer, Input* input) {
    Player* player = malloc(sizeof(Player));

    player->_level = level;
    player->_renderer = renderer;
    player->_input = input;
    player->on_ground = 0;

    player->size = PLAYER_SIZE_MEDIUM;
    player->position.x = 2.0f;
    player->position.y = 2.0f;

    player->acceleration.x = 0.0f;
    player->acceleration.y = 0.0f;
    player->velocity.x = 0.0f;
    player->velocity.y = 0.0f;

    Sprite* sprite = renderer_load_sprite(player->_renderer, "/player");
    for (int i = 0; i < 3; i++) {
        player->_sprites[i] = sprite;
    }

    return player;
}

void player_update(Player* player, float time_delta) {
    int query_min_x = (int)floor(player->position.x - player_hit_sizes[player->size].x / 2.0f); // left
    int query_max_x = (int)floor(player->position.x + player_hit_sizes[player->size].x / 2.0f); // right
    int query_min_y = (int)floor(player->position.y - player_hit_sizes[player->size].y); // top
    (void)query_min_y;
    int query_max_y = (int)floor(player->position.y); // bottom

    for (int x = query_min_x; x < query_max_x; x++) {
        Tile* tile = level_get_tile(player->_level, x, query_max_y);

        if (tile == NULL) continue;

        if ((tile->bits & TILE_BIT_SOLID) == TILE_BIT_SOLID && !player->on_ground) {
            player->position.y = (float) query_max_y;
            player->on_ground = 1;
        }
    }

    if (player->on_ground) {
        player->velocity.y = 0;

        if (input_button_is_down(player->_input, CONTROLLER_1, CONTROLLER_BUTTON_A)) {
            player->velocity.y = -10.0f;
            player->on_ground = 0;
        }
    }

    if (!player->on_ground) {
        player->velocity.y += player->_level->gravity * time_delta;
    }

    player->acceleration.x = player->velocity.x * time_delta;
    player->acceleration.y = player->velocity.y * time_delta;

    player->position.x += player->acceleration.x;
    player->position.y += player->acceleration.y;
}

void player_draw(Player* player) {
    int draw_pos_x = (int)(player->position.x * sprite_horizontal_frame_size(player->_level->tile_set.sprite));
    int draw_pos_y = (int)(player->position.y * sprite_vertical_frame_size(player->_level->tile_set.sprite));

    Sprite* sprite = player->_sprites[player->size];
    draw_pos_x -= (sprite_width(sprite) / 2);
    draw_pos_y -= (sprite_height(sprite));

    renderer_draw_sprite(player->_renderer, sprite, draw_pos_x, draw_pos_y, 0);
}
