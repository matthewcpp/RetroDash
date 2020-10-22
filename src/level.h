#ifndef LEVEL_H
#define LEVEL_H

#include "entity.h"
#include "renderer.h"
#include "tilemap.h"

typedef struct {
    Entity* entities[2];
    float speed;
    float gravity;

    Renderer* _renderer;
    Tilemap* tilemap;
} Level;

Level* level_create(Renderer* renderer);
void level_update(Level* level, float time);
void level_draw(Level* level);

#endif