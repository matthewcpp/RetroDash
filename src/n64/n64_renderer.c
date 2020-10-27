#include "../renderer.h"
#include "n64_renderer.h"

#include <libdragon.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Renderer* n64_renderer_create() {
    Renderer* renderer = calloc(1, sizeof(Renderer));
    renderer->clear_color = graphics_make_color(255, 255, 255, 255);
    renderer->draw_mode = DDRAW_MODE_UNSPECIFIED;

    for (int i = 0; i < BATCH_COUNT; i++) {
        renderer->tile_batches[i] = tile_batch_create();
    }

    return renderer;
}

Sprite* renderer_load_sprite(Renderer* renderer, const char* path) {
    char* spritePath = malloc(strlen(path) + 8);  // .sprite
    sprintf(spritePath, "%s.sprite", path);

    int handle = dfs_open(spritePath);
    free(spritePath);

    if (handle < 0)
        return NULL;

    sprite_t* libdragon_sprite = malloc( dfs_size( handle ) );
    dfs_read(libdragon_sprite, 1, dfs_size( handle ), handle);
    dfs_close(handle);

    Sprite* sprite = malloc(sizeof(Sprite*));
    sprite->libdragon_sprite = libdragon_sprite;

    return sprite;
}

static void renderer_enable_texture_mode(Renderer* renderer) {
    if (renderer->draw_mode != DRAW_MODE_TEXTURED) {
        rdp_enable_texture_copy();
        rdp_sync( SYNC_PIPE );
        renderer->draw_mode = DRAW_MODE_TEXTURED;
    }
}

void renderer_draw_sprite(Renderer* renderer, Sprite* sprite, int x, int y, int frame) {
    renderer_enable_texture_mode(renderer);

    rdp_sync( SYNC_PIPE );
    rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, sprite->libdragon_sprite, frame);
    rdp_draw_sprite( 0, x, y, MIRROR_DISABLED );
}

void renderer_set_color(Renderer* renderer, int r, int g, int b, int a) {
    uint32_t color = graphics_make_color(r, g, b, a);
    rdp_set_primitive_color(color);
}

void renderer_draw_filled_rect(Renderer* renderer, Rect* rect) {
    rdp_sync( SYNC_PIPE );
    rdp_draw_filled_rectangle(rect->x, rect->y, rect->x + rect->w, rect->y + rect->w);
}

void renderer_begin_tile_drawing(Renderer* renderer, Sprite* sprite) {
    for (int i = 0; i < BATCH_COUNT; i++) {
        renderer->tile_batches[i]->count = 0;
    }

    renderer->tile_sprite = sprite;
}

void renderer_draw_tile(Renderer* renderer, int index, int x, int y) {
    tile_batch_add(renderer->tile_batches[index], x, y);
}

void renderer_end_tile_drawing(Renderer* renderer) {
    renderer_enable_texture_mode(renderer);

    for (int b = 0; b < BATCH_COUNT; b++) {
        TileBatch* batch = renderer->tile_batches[b];

        if (batch->count == 0) continue;

        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, renderer->tile_sprite->libdragon_sprite, b);

        for (int i = 0; i < batch->count; i++) {
            rdp_draw_sprite( 0, batch->positions[i].x, batch->positions[i].y, MIRROR_DISABLED );
        }

        rdp_sync( SYNC_PIPE ); // flush draw commands before loading next batch texture
    }

    renderer->tile_sprite = NULL;
}

void renderer_set_clear_color(Renderer* renderer, uint8_t r, uint8_t g, uint8_t b) {
    renderer->clear_color = graphics_make_color(r, g, b, 255);
}

int sprite_width(Sprite* sprite) {
    return sprite->libdragon_sprite->width;
}

int sprite_height(Sprite* sprite) {
    return sprite->libdragon_sprite->height;
}

int sprite_horizontal_frame_size(Sprite* sprite){
    return sprite->libdragon_sprite->width / sprite->libdragon_sprite->hslices;
}

int sprite_vertical_frame_size(Sprite* sprite) {
    return sprite->libdragon_sprite->height / sprite->libdragon_sprite->vslices;
}