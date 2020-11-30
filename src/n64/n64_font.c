#include "n64_renderer.h"

#include "../font_private.h"
#include "../rect.h"

#include <libdragon.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Font {
    uint32_t font_size;
    uint32_t glyph_count;
    sprite_t* surface;
    FontGlyphInfo* glyphs;
};

Font* renderer_load_font(Renderer* renderer, const char* font_base_path) {
    char path_buffer[64];
    sprintf(path_buffer, "%s.sprite", font_base_path);

    int sprite_handle = dfs_open(path_buffer);
    if (sprite_handle < 0)
        return NULL;

    sprintf(path_buffer, "%s.font", font_base_path);
    int font_handle = dfs_open(path_buffer);
    if (font_handle < 0) {
        dfs_close(sprite_handle);
        return NULL;
    }

    Font* font = malloc(sizeof(Font));
    font->surface = malloc( dfs_size( sprite_handle ) );
    dfs_read(font->surface, 1, dfs_size( sprite_handle ), sprite_handle);
    dfs_close(sprite_handle);

    dfs_read(&font->font_size, sizeof(uint32_t), 1, font_handle);
    dfs_read(&font->glyph_count, sizeof(uint32_t), 1, font_handle);
    font->glyphs = malloc(sizeof(FontGlyphInfo) * font->glyph_count);
    dfs_read(font->glyphs, sizeof(FontGlyphInfo), font->glyph_count, font_handle);
    dfs_close(font_handle);

    return font;
}

// This function assumes a 16bpp image!
void copy_sprite(sprite_t* src_sprite, Rect* src_rect, sprite_t* dst_sprite, Point* dest_point) {
    uint16_t* src_data = (uint16_t*)&src_sprite->data[0];
    uint16_t* dest_data = (uint16_t*)&dst_sprite->data[0];

    for (int row = 0; row < src_rect->h; row++) {
        for (int col = 0; col < src_rect->w; col++) {
            int src_index = (src_sprite->width * (src_rect->y + row)) + src_rect->x + col;
            int dest_index = (dst_sprite->width * (dest_point->y + row)) + dest_point->x + col;

            dest_data[dest_index] = src_data[src_index];
        }
    }
}

Sprite* renderer_create_text_sprite(Renderer* renderer, Font* font, const char* str) {
    int width = measure_text_width(font->glyphs, font->glyph_count, str);
    size_t sprite_size = (width * font->font_size) * sizeof(uint16_t);
    sprite_t* text_sprite = calloc(1, sizeof(sprite_t) + sprite_size);

    text_sprite->width = width;
    text_sprite->height = font->font_size;
    text_sprite->bitdepth = font->surface->bitdepth;
    text_sprite->format = font->surface->format;
    text_sprite->hslices = (sprite_size / 2048) + 1;
    text_sprite->vslices = 1;

    FontGlyphInfo* glyph = find_font_glyph(font->glyphs, font->glyph_count, str[0]);
    int x = glyph->left;

    do {
        Rect source_rect;
        source_rect.x = glyph->x;
        source_rect.y = glyph->y;
        source_rect.w = glyph->width + glyph->left;
        source_rect.h = glyph->top + glyph->bottom;

        Point dest_point;
        dest_point.x = x - glyph->left;
        dest_point.y = -glyph->top;

        copy_sprite(font->surface, &source_rect, text_sprite, &dest_point);

        x += glyph->width + CHAR_SPACER;
        str += 1;

        glyph = find_font_glyph(font->glyphs, font->glyph_count, str[0]);
    } while (str[0] != '\0');

    Sprite* sprite = malloc(sizeof(Sprite));
    sprite->libdragon_sprite = text_sprite;

    return sprite;
}

void renderer_destroy_font(Renderer* renderer, Font* font) {
    free(font->surface);
    free(font->glyphs);
    free(font);
}

int font_size(Font* font) {
    return font->font_size;
}
