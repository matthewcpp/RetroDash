#include "renderer.h"
#include "n64_renderer.h"

#include <libdragon.h>

#include <stdlib.h>
#include <stdint.h>

Renderer* n64_renderer_create() {
    Renderer* renderer = calloc(1, sizeof(Renderer));

    return renderer;
}

Sprite* n64_rendrer_load_sprite(Renderer* renderer, const char* path) {
    (void)renderer;
    int fp = dfs_open(path);
    sprite_t* libdragon_sprite = malloc( dfs_size( fp ) );
    dfs_read( libdragon_sprite, 1, dfs_size( fp ), fp );
    dfs_close( fp );

    Sprite* sprite = malloc(sizeof(Sprite*));
    sprite->libdragon_sprite = libdragon_sprite;

    return sprite;
}

void n64_renderer_load_sprites(Renderer* renderer) {
    renderer->sprites[0] = n64_rendrer_load_sprite(renderer, "/player.sprite");
}

void renderer_draw_sprite(Renderer* renderer, Sprite* sprite, int x, int y, int frame) {
    (void)renderer;
    rdp_sync( SYNC_PIPE );
    rdp_load_texture_stride( 0, 0, MIRROR_DISABLED, sprite->libdragon_sprite, frame);
    rdp_draw_sprite( 0, x, y, MIRROR_DISABLED );
}

Sprite* renderer_get_sprite(Renderer* renderer, int index) {
    return renderer->sprites[index];
}

void renderer_set_color(Renderer* renderer, int r, int g, int b, int a) {
    uint32_t color = graphics_make_color(r, g, b, a);
    rdp_set_primitive_color(color);
}

void renderer_draw_filled_rect(Renderer* renderer, Rect* rect) {
    rdp_sync( SYNC_PIPE );
    rdp_draw_filled_rectangle(rect->x, rect->y, rect->x + rect->w, rect->y + rect->w);
}
