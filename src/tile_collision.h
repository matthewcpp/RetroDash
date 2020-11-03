#ifndef TILE_COLLISION_H
#define TILE_COLLISION_H

#include "entity.h"

/**
 * Checks collision between an entity and a spike tile.
 * Spike tile is described as a triangle bounded in the tile at the given position
 */
int tile_collision_spike(Entity* entity, int pos_x, int pos_y);

#endif