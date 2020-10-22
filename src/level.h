#ifndef LEVEL_H
#define LEVEL_H

#include "entity.h"
#include "renderer.h"

typedef struct {
    Entity* entity;
    float speed;
    float gravity;

    Renderer* _renderer;
} Level;

Level* level_create(Renderer* renderer);
void level_update(Level* level, float time);
void level_draw(Level* level);

#endif