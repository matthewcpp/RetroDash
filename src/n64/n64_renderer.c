#include "../renderer.h"
#include "n64_renderer.h"

#include <libdragon.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Renderer* n64_renderer_create(int screen_width, int screen_height) {
    Renderer* renderer = calloc(1, sizeof(Renderer));
    renderer->display_context = 0;
    renderer->clear_color = graphics_make_color(255, 255, 255, 255);
    renderer->primitive_color = graphics_make_color(0, 0, 0, 255);
    renderer->draw_mode = DDRAW_MODE_UNSPECIFIED;

    renderer->screen_size.x = screen_width;
    renderer->screen_size.y = screen_height;

    renderer->tile_batches = NULL;
    renderer->tile_batch_count = 0;
    software_tile_batch_init(&renderer->software_tiles);

    return renderer;
}

void renderer_get_screen_size(Renderer* renderer, Point* screen_size) {
    screen_size->x = renderer->screen_size.x;
    screen_size->y = renderer->screen_size.y;
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

    Sprite* sprite = malloc(sizeof(Sprite));
    sprite->libdragon_sprite = libdragon_sprite;

    return sprite;
}

static void renderer_enable_filled_mode(Renderer* renderer) {
    if (renderer->draw_mode != DRAW_MODE_FILLED) {
        rdp_sync( SYNC_PIPE );
        rdp_enable_primitive_fill();
        renderer->draw_mode = DRAW_MODE_FILLED;
    }
}

static void renderer_enable_texture_mode(Renderer* renderer) {
    if (renderer->draw_mode != DRAW_MODE_TEXTURED) {
        rdp_sync( SYNC_PIPE );
        rdp_enable_texture_copy();
        renderer->draw_mode = DRAW_MODE_TEXTURED;
    }
}

void renderer_draw_sprite_row(Renderer* renderer, Sprite* sprite, int row, int x, int y) {
    renderer_enable_texture_mode(renderer);

    int pos_x = x;
    int pos_y = y;

    int stride_x = sprite->libdragon_sprite->width / sprite->libdragon_sprite->hslices;
    int base_index = sprite->libdragon_sprite->hslices * row;

    for (int i = 0; i < sprite->libdragon_sprite->hslices; i++) {
        rdp_sync( SYNC_PIPE );
        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, sprite->libdragon_sprite, base_index + i);
        rdp_draw_sprite( 0, pos_x, pos_y, MIRROR_DISABLED );

        pos_x += stride_x;
    }
}

void renderer_draw_sprite(Renderer* renderer, Sprite* sprite, int x, int y) {
    renderer_enable_texture_mode(renderer);

    int pos_x = x;
    int pos_y = y;

    int stride_x = sprite->libdragon_sprite->width / sprite->libdragon_sprite->hslices;
    int stride_y = sprite->libdragon_sprite->height / sprite->libdragon_sprite->vslices;

    for (int v = 0; v < sprite->libdragon_sprite->vslices; v++) {
        for (int h = 0; h < sprite->libdragon_sprite->hslices; h++) {
            int frame = v * sprite->libdragon_sprite->hslices + h;

            rdp_sync( SYNC_PIPE );
            rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, sprite->libdragon_sprite, frame);
            rdp_draw_sprite( 0, pos_x, pos_y, MIRROR_DISABLED );

            pos_x += stride_x;
        }

        pos_x = x;
        pos_y += stride_y;
    }
}

void renderer_destroy_sprite(Renderer* renderer, Sprite* sprite) {
    free(sprite);
    (void)renderer;
}


void renderer_draw_scaled_sprite(Renderer* renderer, Sprite* sprite,  int x, int y, float scale_x, float scale_y, int frame) {
    renderer_enable_texture_mode(renderer);

    rdp_sync( SYNC_PIPE );
    rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, sprite->libdragon_sprite, frame);
    rdp_draw_sprite_scaled( 0, x, y, scale_x, scale_y, MIRROR_DISABLED );
}

void renderer_draw_sprite_frame(Renderer* renderer, Sprite* sprite, int frame, int x, int y) {
    renderer_enable_texture_mode(renderer);

    rdp_sync( SYNC_PIPE );
    rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, sprite->libdragon_sprite, frame);
    rdp_draw_sprite( 0, x, y, MIRROR_DISABLED );
}

void renderer_set_color(Renderer* renderer, int r, int g, int b, int a) {
    renderer->primitive_color = graphics_make_color(r, g, b, a);
    rdp_set_primitive_color(renderer->primitive_color);
}

void renderer_draw_filled_rect(Renderer* renderer, Rect* rect) {
    renderer_enable_filled_mode(renderer);

    rdp_sync( SYNC_PIPE );
    rdp_draw_filled_rectangle(rect->x, rect->y, rect->x + rect->w, rect->y + rect->h);
}

#define GRID_SIZE 32

void renderer_draw_grid(Renderer* renderer) {
    renderer_enable_filled_mode(renderer);

    Rect rect;
    rect_set(&rect, 0, GRID_SIZE, renderer->screen_size.x, 0);

    while (rect.y < renderer->screen_size.y) {
        rdp_draw_filled_rectangle(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
        rect.y += GRID_SIZE;
    }

    rect_set(&rect, GRID_SIZE, 0, 0, renderer->screen_size.y);

    while (rect.x < renderer->screen_size.x) {
        rdp_draw_filled_rectangle(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
        rect.x += GRID_SIZE;
    }
}

static void renderer_clear_tile_batches(Renderer* renderer) {
    for (int i = 0; i < renderer->tile_batch_count; i++)
        tile_batch_destroy(renderer->tile_batches[i]);

    free(renderer->tile_batches);
    renderer->tile_batch_count = 0;
    renderer->tile_batches = NULL;
}

// TODO: Only deallocate if necessary?
void renderer_set_tile_batch_size(Renderer* renderer, int size) {
    if (renderer->tile_batches) {
        renderer_clear_tile_batches(renderer);
    }

    renderer->tile_batch_count = size;
    renderer->tile_batches = calloc(renderer->tile_batch_count, sizeof(TileBatch*));
    for (int i = 0; i < renderer->tile_batch_count; i++)
        renderer->tile_batches[i] = tile_batch_create();
}

void renderer_begin_tile_drawing(Renderer* renderer, Sprite* sprite) {
    for (int i = 0; i < renderer->tile_batch_count; i++) {
        renderer->tile_batches[i]->count = 0;
    }

    renderer->tile_sprite = sprite;
}

void renderer_draw_tile(Renderer* renderer, int index, int x, int y) {
    if (x >= 0)
        tile_batch_add(renderer->tile_batches[index], x, y);
    else
        software_tile_batch_add(&renderer->software_tiles, index, x, y);
}

void renderer_end_tile_drawing(Renderer* renderer) {
    renderer_enable_texture_mode(renderer);

    for (int b = 0; b < renderer->tile_batch_count; b++) {
        TileBatch* batch = renderer->tile_batches[b];

        if (batch->count == 0) continue;

        rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, renderer->tile_sprite->libdragon_sprite, b);

        for (int i = 0; i < batch->count; i++) {
            rdp_draw_sprite( 0, batch->positions[i].x, batch->positions[i].y, MIRROR_DISABLED );
        }

        rdp_sync( SYNC_PIPE ); // flush draw commands before loading next batch texture
    }
}

void n64_renderer_draw_software_tiles(Renderer* renderer) {
    for (int i =0; i < renderer->software_tiles.count; i++) {
        SoftwareTile* tile = renderer->software_tiles.tiles + i;

        graphics_draw_sprite_stride(renderer->display_context, tile->x, tile->y, renderer->tile_sprite->libdragon_sprite, tile->frame);
    }

    renderer->software_tiles.count = 0;
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