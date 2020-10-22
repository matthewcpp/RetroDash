#include "player.h"

#include <stdlib.h>

#define PLAYER_SPRITE_INDEX 0
#define FRAME_TIME 1.0f / 20.0f

Player* player_create(Level* level, Input* input, Renderer* renderer) {
    Player* player = malloc(sizeof(Player));

    player->_level = level;
    player->_input = input;
    player->_renderer = renderer;

    player->pos_x = 0.0f;
    player->pos_y = 0.0f;

    player->frame_time = 0.0f;
    player->sprite_frame = 0;

    player->jump_velocity = 200.0f;
    player->velocity = 0.0f;
    player->on_ground = 0;

    return player;
}

void player_update(Player* player, float time) {
    player->frame_time += time;
    if (player->frame_time > FRAME_TIME) {
        player->frame_time -= FRAME_TIME;
        player->sprite_frame += 1;

        if (player->sprite_frame > 3)
            player->sprite_frame = 1;
    }


    if (player->on_ground && input_button_is_down(player->_input, CONTROLLER_1, CONTROLLER_BUTTON_A)){
        player->on_ground = 0;
        player->velocity = player->jump_velocity;
    }

    if (!player->on_ground) {
        player->pos_y -= player->velocity * time;
        player->velocity -= player->_level->gravity * time;

        if (player->pos_y >= 175.0f) {
            player->pos_y = 175.0f;
            player->on_ground = 1;
        }
    }

}

void player_draw(Player* player) {
    Sprite* sprite = renderer_get_sprite(player->_renderer, PLAYER_SPRITE_INDEX);
    renderer_draw_sprite(player->_renderer, sprite, (int)player->pos_x, (int)player->pos_y, player->sprite_frame);
}
