#include "level.h"

#include <stdlib.h>

Level* level_create(Renderer* renderer) {
    Level* level = malloc(sizeof(Level));

    level->entity = entity_create(renderer);
    level->entity->rect.x = 320.0f;
    level->entity->rect.y = 200.0f;
    level->entity->rect.w = 32.0f;
    level->entity->rect.h = 40.0f;

    level->speed = 50.0f;
    level->_renderer = renderer;
    level->gravity = 800.0f;

    return level;
}

void level_update(Level* level, float time) {
    float delta = level->speed * time;
    level->entity->rect.x -= delta;

    if (level->entity->rect.x + level->entity->rect.w < 0.0f)
        level->entity->rect.x = 320;
}

void level_draw(Level* level) {
    entity_draw(level->entity);
}
