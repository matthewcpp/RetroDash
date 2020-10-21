#include "player.h"

#include <stdlib.h>

#define PLAYER_SPRITE_INDEX 0
#define FRAME_TIME 1.0f / 20.0f
#define PLAYER_SPEED 75.0f

Player* player_create(Input* input, Renderer* renderer) {
    Player* player = malloc(sizeof(Player));

    player->_input = input;
    player->_renderer = renderer;

    player->_prev_x = 0.0f;
    player->_prev_y = 0.0f;
    player->pos_x = 0.0f;
    player->pos_y = 0.0f;

    player->frame_time = 0.0f;
    player->sprite_frame = 0;

    return player;
}

static int did_move(Player* player) {
    return player->_prev_x != player->pos_x || player->_prev_y != player->pos_y;
}

static void update_anim(Player* player, float time) {
    if (did_move(player)) {
        player->frame_time += time;
        if (player->frame_time > FRAME_TIME) {
            player->frame_time -= FRAME_TIME;
            player->sprite_frame += 1;

            if (player->sprite_frame > 3)
                player->sprite_frame = 1;
        }
    }
    else {
        player->sprite_frame = 0;
        player->frame_time = FRAME_TIME;
    }
}

void player_update(Player* player, float time) {
    if (input_button_is_held(player->_input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_RIGHT))
        player->pos_x += time * PLAYER_SPEED;
    else if (input_button_is_held(player->_input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_LEFT))
        player->pos_x -= time * PLAYER_SPEED;

    else if (input_button_is_held(player->_input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_UP))
        player->pos_y -= time * PLAYER_SPEED;
    else if (input_button_is_held(player->_input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_DOWN))
        player->pos_y += time * PLAYER_SPEED;

    update_anim(player, time);

    player->_prev_x = player->pos_x;
    player->_prev_y = player->pos_y;
}

void player_draw(Player* player) {
    Sprite* sprite = renderer_get_sprite(player->_renderer, PLAYER_SPRITE_INDEX);
    renderer_draw_sprite(player->_renderer, sprite, (int)player->pos_x, (int)player->pos_y, player->sprite_frame);
}
