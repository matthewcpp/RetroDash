#ifndef BATCH_H
#define BATCH_H

typedef struct {
    int x;
    int y;
} TilePos;

typedef struct {
    int count;
    int capacity;
    TilePos* positions;
} TileBatch;

TileBatch* tile_batch_create();
void tile_batch_add(TileBatch* batch, int x, int y);


#endif