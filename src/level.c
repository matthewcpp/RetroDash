#include "level.h"

#include "filesystem.h"

#include <stdlib.h>

Level* level_create(Renderer* renderer) {
    Level* level = malloc(sizeof(Level));

    level->_renderer = renderer;
    level->name = "";

    level->tile_set.sprite = NULL;
    level->tile_set.palette_size = 0;
    level->tile_set.palette = NULL;

    return level;
}

int level_set_tile(Level* level, int x, int y, int tile) {
    level->_tile_map[y * level->width + x] = tile;

    return 1;
}

void level_draw(Level* level) {
    renderer_begin_tile_drawing(level->_renderer, level->tile_set.sprite);

    int tile_width = sprite_horizontal_frame_size(level->tile_set.sprite);
    int tile_height = sprite_horizontal_frame_size(level->tile_set.sprite);

    for (int y = 0; y < level->height; y++) {
        for (int x = 0; x < level->width; x++) {
            int index = y * level->width + x;

            int palette_index = level->_tile_map[index];
            if (palette_index == TILE_EMPTY) continue;

            // TODO: This will need to be updated when camera moves
            renderer_draw_tile(level->_renderer,
                               level->tile_set.palette[palette_index].sprite_index,
                               x * tile_width,
                               y * tile_height);
        }
    }

    renderer_end_tile_drawing(level->_renderer);
}

static int load_tile_set(Level* level, const char* path) {
    uint32_t tilemap_file = filesystem_open(path);
    if (tilemap_file < 0) return 0;

    uint32_t sprite_name_size;
    filesystem_read(&sprite_name_size, sizeof(uint32_t), 1, tilemap_file);
    char* sprite_name = malloc(sprite_name_size + 1);
    filesystem_read(sprite_name, 1, sprite_name_size, tilemap_file);
    sprite_name[sprite_name_size] = '\0';

    level->tile_set.sprite = renderer_load_sprite(level->_renderer, sprite_name);

    if (level->tile_set.sprite == NULL)
        return 0;

    filesystem_read(&level->tile_set.sprite_horizontal_slices, sizeof(uint32_t), 1, tilemap_file);
    filesystem_read(&level->tile_set.sprite_vertical_slices, sizeof(uint32_t), 1, tilemap_file);

    filesystem_read(&level->tile_set.palette_size, sizeof(uint32_t), 1, tilemap_file);
    level->tile_set.palette = malloc(sizeof(Tile) * level->tile_set.palette_size);
    filesystem_read(level->tile_set.palette, sizeof(Tile), level->tile_set.palette_size, tilemap_file);

    filesystem_close(tilemap_file);
    return 1;
}

int level_load(Level* level, const char* path) {
    uint32_t level_file = filesystem_open(path);
    if (level_file < 0) return 0;

    // Read level name
    uint32_t size;
    filesystem_read(&size, sizeof(uint32_t), 1, level_file);
    level->name = malloc(size + 1);
    filesystem_read(level->name, 1, size, level_file);
    level->name[size] = '\0';

    // Read tile map
    filesystem_read(&size, sizeof(uint32_t), 1, level_file);
    char* tile_set_name = malloc(size + 1);
    filesystem_read(tile_set_name, 1, size, level_file);
    tile_set_name[size] = '\0';

    // Read Tiles
    filesystem_read(&level->width, sizeof(uint32_t), 1, level_file);
    filesystem_read(&level->height, sizeof(uint32_t), 1, level_file);

    level->_tile_map = malloc(level->width * level->height);
    filesystem_read(level->_tile_map, 1, level->width * level->height, level_file);

    filesystem_close(level_file);

    int loaded_tile_set = load_tile_set(level, tile_set_name);
    free(tile_set_name);

    return loaded_tile_set;
}