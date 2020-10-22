#include "level.h"

#include <stdlib.h>

void create_tilemap(Level* level);

Level* level_create(Renderer* renderer) {
    Level* level = malloc(sizeof(Level));

    level->tilemap = tilemap_create();
    level->speed = 50.0f;
    level->_renderer = renderer;
    level->gravity = 550.0f;

    create_tilemap(level);

    return level;
}

void level_update(Level* level, float time) {
    float delta = level->speed * time;

    for (int i  =0; i < 2; i++) {
        level->entities[i]->rect.x -= delta;
    }
}

void level_draw(Level* level) {
    
    tilemap_draw(level->tilemap, level->_renderer);

    for (int i  =0; i < 2; i++) {
        // seems to crash if trying to draw a rectangle totally off screen?
        if (level->entities[i]->rect.x + level->entities[i]->rect.w <= 0.0f)
            level->entities[i]->rect.x = 320.0f;

        entity_draw(level->entities[i]);
    }
}

void create_tilemap(Level* level) {
    tilemap_init(level->tilemap, 16, 20, 15);

    for (int i = 0; i < 4; i++)
        level->tilemap->palette[i].sprite_index = i;

    for (int x = 0; x < level->tilemap->width; x++) {
        tilemap_set_tile(level->tilemap, x, level->tilemap->height - 1, 0);
    }

        for (int x = 0; x < level->tilemap->width; x++) {
        tilemap_set_tile(level->tilemap, x, 0, 2);
    }
}