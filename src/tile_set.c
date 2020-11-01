#include "tile_set.h"

#include "filesystem.h"

#include <stdlib.h>

int tile_set_load(TileSet* tile_set, const char* path, Renderer* renderer) {
    uint32_t tilemap_file = filesystem_open(path);
    if (tilemap_file < 0) return 0;

    uint32_t sprite_name_size;
    filesystem_read(&sprite_name_size, sizeof(uint32_t), 1, tilemap_file);
    char* sprite_name = malloc(sprite_name_size + 1);
    filesystem_read(sprite_name, 1, sprite_name_size, tilemap_file);
    sprite_name[sprite_name_size] = '\0';

    tile_set->sprite = renderer_load_sprite(renderer, sprite_name);
    free(sprite_name);

    if (tile_set->sprite == NULL){
        filesystem_close(tilemap_file);
        return 0;
    }

    filesystem_read(&tile_set->palette_size, sizeof(uint32_t), 1, tilemap_file);
    tile_set->palette = malloc(sizeof(Tile) * tile_set->palette_size);
    filesystem_read(tile_set->palette, sizeof(Tile), tile_set->palette_size, tilemap_file);

    filesystem_close(tilemap_file);
    return 1;
}

void tile_set_clear(TileSet* tile_set, Renderer* renderer) {
    if (tile_set->palette) {
        free(tile_set->palette);
        tile_set->palette = NULL;
    }

    if (tile_set->sprite) {
        renderer_destroy_sprite(renderer, tile_set->sprite);
        tile_set->sprite = NULL;
    }
}