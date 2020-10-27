#ifndef PLAYER_H
#define PLAYER_H

#include "level.h"
#include "rect.h"
#include "renderer.h"

typedef enum {
    PLAYER_SIZE_SMALL,
    PLAYER_SIZE_MEDIUM,
    PLAYER_SIZE_LARGE
} PlayerSize;

typedef struct {
    PlayerSize size;

    Point position;

    Sprite*_sprites[3];
    Level* _level;
    Renderer* _renderer;
} Player;

Player* player_create(Level* level, Renderer* renderer);
void player_update(Player* player);
void player_draw(Player* player);

#endif