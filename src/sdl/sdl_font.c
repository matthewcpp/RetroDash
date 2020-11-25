#include "sdl_renderer.h"

#include <SDL_image.h>

#include <stdio.h>

typedef struct {
    /** positive numbers indicating a distance going up from the given baseline. */
    int8_t top;

    /** positive numbers indicating a distance going down from the given baseline. */
    int8_t bottom;

    /** positive numbers indicating a distance going left from the given alignment point. */
    int8_t left;

    /** the width of the rendering context's scratch bitmap, in CSS pixels */
    int8_t width;

    uint16_t x;
    uint16_t y;
} FontGlyphInfo;

struct Font {
    uint32_t size;
    SDL_Surface* surface;
    FontGlyphInfo* glyphs;
};

Font* renderer_load_font(Renderer* renderer, const char* font_base_path) {
    char* path_buffer = malloc(strlen(renderer->asset_dir) + strlen(font_base_path) + 12); //.sprite, .font

    sprintf(path_buffer, "%s%s.font", renderer->asset_dir, font_base_path);
    FILE* font_info_file = fopen(path_buffer, "rb");

    if (!font_info_file) return NULL;

    sprintf(path_buffer, "%s%s.png", renderer->asset_dir, font_base_path);
    SDL_Surface* surface = IMG_Load(path_buffer);
    if (!surface) {
        fclose(font_info_file);
        return NULL;
    }

    Font* font = malloc(sizeof(Font));
    font->surface = surface;

    fread(&font->size, sizeof(uint32_t), 1, font_info_file);

    uint32_t glyph_count;
    fread(&glyph_count, sizeof(uint32_t), 1, font_info_file);
    font->glyphs = malloc(sizeof(FontGlyphInfo) * glyph_count);
    fread(font->glyphs, sizeof(FontGlyphInfo), glyph_count, font_info_file);

    return font;
}

static int measure_text_width(Font* font, const char* str) {
    int index = str[0] - ' ';
    int width = font->glyphs[index].left;

    while (str[0] != '\0') {
        index = str[0] - ' ';
        width += font->glyphs[index].width;

        str += 1;
    }

    return width;
}

Sprite* renderer_create_text_sprite(Renderer* renderer, Font* font, const char* str) {
    int width = measure_text_width(font, str);
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, width, font->size, 32, SDL_PIXELFORMAT_RGBA32);

    int index = str[0] - ' ';
    FontGlyphInfo* glyph = font->glyphs + index;
    int x = glyph->left;

    while (str[0] != '\0') {
        index = str[0] - ' ';
        glyph = font->glyphs + index;

        SDL_Rect source_rect;
        source_rect.x = glyph->x;
        source_rect.y = glyph->y;
        source_rect.w = glyph->width + glyph->left;
        source_rect.h = glyph->top + glyph->bottom;

        SDL_Rect dest_rect;
        dest_rect.x = x - glyph->left;
        dest_rect.y = -glyph->top;

        SDL_BlitSurface(font->surface, &source_rect, surface, &dest_rect);

        x += glyph->width;
        str += 1;
    }

    Sprite* text_sprite = malloc(sizeof(Sprite));
    text_sprite->texture = SDL_CreateTextureFromSurface(renderer->sdl_renderer, surface);
    text_sprite->width = width;
    text_sprite->height = font->size;
    text_sprite->horizontal_slices = 1;
    text_sprite->vertical_slices = 1;

    SDL_FreeSurface(surface);

    return text_sprite;
}

void renderer_destroy_font(Renderer* renderer, Font* font) {
    SDL_FreeSurface(font->surface);
    free(font->glyphs);
    free(font);
}