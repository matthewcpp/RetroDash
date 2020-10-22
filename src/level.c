#include "level.h"

#include <stdlib.h>

Level* level_create(Renderer* renderer) {
    Level* level = malloc(sizeof(Level));

    level->speed = 50.0f;
    level->_renderer = renderer;
    level->gravity = 550.0f;

    return level;
}

void level_update(Level* level, float time) {
    float delta = level->speed * time;

    for (int i  =0; i < 2; i++) {
        level->entities[i]->rect.x -= delta;
    }
}

void level_draw(Level* level) {
    for (int i  =0; i < 2; i++) {
        // seems to crash if trying to draw a rectangle totally off screen?
        if (level->entities[i]->rect.x + level->entities[i]->rect.w <= 0.0f)
            level->entities[i]->rect.x = 320.0f;

        entity_draw(level->entities[i]);
    }
}
