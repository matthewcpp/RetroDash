#ifndef LEVEL_H
#define LEVEL_H

#include "audio.h"
#include "camera.h"
#include "particles.h"
#include "renderer.h"
#include "tile_set.h"

#include <stdint.h>

#define TILE_EMPTY UINT8_MAX

typedef struct {
    uint32_t width;
    uint32_t height;
    Vec2 start_pos;
    float goal_dist;
    char* name;
    float gravity;
    BrickParticles brick_particles;
    Music* music;

    TileSet tile_set;
    uint8_t* _tile_map;

    Renderer* _renderer;
    Audio* _audio;
    Camera* _camera;
    int _file_handle;
} Level;

Level* level_create(Audio* audio, Renderer* renderer, Camera* camera);
void level_destroy(Level* level);

int level_load(Level* level, const char* path);
Tile* level_get_tile(Level* level, int x, int y);
void level_update(Level* level, float time_delta);

/**
 * Resets the level to it's initial state.  Does not reload tileset information.
 * Used after the player has been killed to reset any grid state that has been modified (i.e. broken bricks)
 */
void level_reset(Level* level);
void level_draw(Level* level);
void level_set_tile(Level* level, int x, int y, uint8_t tile_palette_index);

/**
 * Returns the distance the player must travel to beat the level.
 */
float level_travel_distance(Level* level);

#endif