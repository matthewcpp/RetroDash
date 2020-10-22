#ifndef TILEMAP_H
#define TILEMAP_H

#include "renderer.h"

typedef enum {
    TILE_BIT_NONE = 0,
    TILE_BIT_SOLID = 1
} TileBits;

typedef struct {
    int sprite_index;
    unsigned int bits;
} Tile;

#define PALETTE_SIZE 4

typedef struct {
	int tile_size;
	int width;
	int height;

	Tile palette[PALETTE_SIZE];
	unsigned char* tiles;
} Tilemap;

Tilemap* tilemap_create();
void tilemap_init(Tilemap* tilemap, int tile_size, int width, int height);
int tilemap_set_tile(Tilemap* tilemap, int x, int y, unsigned char tile);

void tilemap_draw(Tilemap* tilemap, Renderer* renderer);

#endif