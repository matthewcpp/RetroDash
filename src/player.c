#include "player.h"

#include <stdlib.h>

//static Point player_size_dimensions[3] = { {16, 16}, {24, 24}, {32, 32} };

Player* player_create(Level* level, Renderer* renderer) {
    Player* player = malloc(sizeof(Player));

    player->_level = level;
    player->_renderer = renderer;

    player->size = PLAYER_SIZE_MEDIUM;
    player->position.x = 50;
    player->position.y = 50;

    Sprite* sprite = renderer_load_sprite(player->_renderer, "/player");
    for (int i = 0; i < 3; i++) {
        player->_sprites[i] = sprite;
    }

    return player;
}

void player_update(Player* player) {

}

void player_draw(Player* player) {
    Sprite* sprite = player->_sprites[player->size];
    int draw_pos_x = player->position.x - (sprite_width(sprite) / 2);
    int draw_pos_y = player->position.y - (sprite_height(sprite) / 2);

    renderer_draw_sprite(player->_renderer, sprite, draw_pos_x, draw_pos_y, 0);
}