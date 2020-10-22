#include "tilemap.h"

#include <stdlib.h>
#include <string.h>

Tilemap* tilemap_create(int tile_size, int width, int height) {
    Tilemap* tilemap = calloc(1, sizeof(Tilemap));

    return tilemap;
}

void tilemap_init(Tilemap* tilemap, int tile_size, int width, int height) {
    // clear the board if it is loaded
    if (tilemap->tiles) {
        free(tilemap->tiles);
    }

    //clear out the palette
    memset(&tilemap->palette[0], 0, sizeof(Tile) * PALETTE_SIZE);

    // create new board
    tilemap->tile_size = tile_size;
    tilemap->width = width;
    tilemap->height = height;

    tilemap->tiles = malloc(width * height);
    memset(tilemap->tiles, 255, width * height);
}

void tilemap_draw(Tilemap* tilemap, Renderer* renderer) {
    renderer_begin_tile_drawing(renderer);

    for (int y = 0; y < tilemap->height; y++) {
        for (int x = 0; x < tilemap->width; x++) {
            int index = y * tilemap->width + x;

            int palette_index = tilemap->tiles[index];
            if (palette_index == 255) continue;

            renderer_draw_tile(renderer, tilemap->palette[palette_index].sprite_index, x * tilemap->tile_size, y * tilemap->tile_size);
        }
    }

    renderer_end_tile_drawing(renderer);
}

int tilemap_set_tile(Tilemap* tilemap, int x, int y, unsigned char tile) {
    tilemap->tiles[y * tilemap->width + x] = tile;

    return 1;
}

