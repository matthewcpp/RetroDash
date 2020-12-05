#include "sdl_renderer.h"

#include "../font_private.h"

#include <SDL_image.h>

#include <stdio.h>

struct Font {
    uint32_t font_size;
    uint32_t glyph_count;
    SDL_Surface* surface;
    FontGlyphInfo* glyphs;
};

Font* renderer_load_font(Renderer* renderer, const char* font_base_path) {
    char* path_buffer = malloc(strlen(renderer->asset_dir) + strlen(font_base_path) + 12); // separator .png, .font

    sprintf(path_buffer, "%s/%s.font", renderer->asset_dir, font_base_path);
    FILE* font_info_file = fopen(path_buffer, "rb");

    if (!font_info_file) return NULL;

    sprintf(path_buffer, "%s/%s.png", renderer->asset_dir, font_base_path);
    SDL_Surface* surface = IMG_Load(path_buffer);
    if (!surface) {
        fclose(font_info_file);
        return NULL;
    }

    Font* font = malloc(sizeof(Font));
    font->surface = surface;

    fread(&font->font_size, sizeof(uint32_t), 1, font_info_file);

    fread(&font->glyph_count, sizeof(uint32_t), 1, font_info_file);
    font->glyphs = malloc(sizeof(FontGlyphInfo) * font->glyph_count);
    fread(font->glyphs, sizeof(FontGlyphInfo), font->glyph_count, font_info_file);
    fclose(font_info_file);

    return font;
}

Sprite* renderer_create_text_sprite(Renderer* renderer, Font* font, const char* str) {
    int width = measure_text_width(font->glyphs, font->glyph_count, str);
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, width, font->font_size, 32, SDL_PIXELFORMAT_RGBA32);

    FontGlyphInfo* glyph = find_font_glyph(font->glyphs, font->glyph_count, str[0]);
    int x = glyph->left;

    do {
        SDL_Rect source_rect;
        source_rect.x = glyph->x;
        source_rect.y = glyph->y;
        source_rect.w = glyph->width + glyph->left;
        source_rect.h = glyph->top + glyph->bottom;

        SDL_Rect dest_rect;
        dest_rect.x = x - glyph->left;
        dest_rect.y = -glyph->top;

        SDL_BlitSurface(font->surface, &source_rect, surface, &dest_rect);

        x += glyph->width + CHAR_SPACER;
        str += 1;

        glyph = find_font_glyph(font->glyphs, font->glyph_count, str[0]);
    } while (str[0] != '\0');

    Sprite* text_sprite = malloc(sizeof(Sprite));
    text_sprite->texture = SDL_CreateTextureFromSurface(renderer->sdl_renderer, surface);
    text_sprite->width = width;
    text_sprite->height = font->font_size;
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

int font_size(Font* font) {
    return font->font_size;
}
