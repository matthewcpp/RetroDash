#include "batch.h"

#include <stdlib.h>

TileBatch* tile_batch_create() {
    TileBatch* tilebatch = malloc(sizeof (TileBatch));

    tilebatch->count = 0;
    tilebatch->capacity = 8;
    tilebatch->positions = calloc(tilebatch->capacity, sizeof(TilePos));

    return tilebatch;
}

void tile_batch_destroy(TileBatch* tile_batch) {
    free(tile_batch->positions);
    free(tile_batch);
}

void tile_batch_add(TileBatch* batch, int x, int y) {
    if (batch->count == batch->capacity) {
        batch->capacity += 8;
        batch->positions = realloc(batch->positions, batch->capacity * sizeof(TilePos));
    }

    batch->positions[batch->count].x = x;
    batch->positions[batch->count].y = y;

    batch->count += 1;
}


void software_tile_batch_init(SoftwareTileBatch* tile_batch) {
    tile_batch->count = 0;
    tile_batch->capacity = 8;
    tile_batch->tiles = calloc(tile_batch->capacity, sizeof(SoftwareTile));
}

void software_tile_batch_uninit(SoftwareTileBatch* tile_batch) {
    free(tile_batch->tiles);
}

void software_tile_batch_add(SoftwareTileBatch* batch, int frame, int x, int y) {
    if (batch->count == batch->capacity) {
        batch->capacity += 8;
        batch->tiles = realloc(batch->tiles, batch->capacity * sizeof(SoftwareTile));
    }

    SoftwareTile* tile = batch->tiles + batch->count;
    tile->frame = frame;
    tile->x = x;
    tile->y = y;

    batch->count += 1;
}
