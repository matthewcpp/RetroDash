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
void tile_batch_destroy(TileBatch* tile_batch);
void tile_batch_add(TileBatch* batch, int x, int y);

/**
 * Libdragon hardware sprites can not be drawn with X < 0.
 * These tiles need to be deferred until after RDP drawing has completed
 * They are then rendered in software.
 */
typedef struct {
    int frame;
    int x;
    int y;
} SoftwareTile;

typedef struct {
    int count;
    int capacity;
    SoftwareTile* tiles;
} SoftwareTileBatch;

void software_tile_batch_init(SoftwareTileBatch* batch);
void software_tile_batch_uninit(SoftwareTileBatch* batch);
void software_tile_batch_add(SoftwareTileBatch* batch, int frame, int x, int y);



#endif