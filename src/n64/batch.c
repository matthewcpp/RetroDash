#include "batch.h"

#include <stdlib.h>

TileBatch* tile_batch_create() {
    TileBatch* tilebatch = malloc(sizeof (TileBatch));

    tilebatch->count = 0;
    tilebatch->capacity = 8;
    tilebatch->positions = calloc(tilebatch->capacity, sizeof(TilePos));

    return tilebatch;
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
