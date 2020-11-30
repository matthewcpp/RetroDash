#include "font_private.h"

#include <stddef.h>

static FontGlyphInfo* find_font_glyph_rec(FontGlyphInfo* glyphs, int min_index, int max_index, uint16_t char_code) {
    uint32_t center_index = min_index + (max_index - min_index) / 2;
    if (min_index > max_index) return NULL;

    FontGlyphInfo* center_glyph = glyphs + center_index;

    if (center_glyph->char_code == char_code) {
        return center_glyph;
    }
    else if (center_glyph->char_code > char_code) {
        return find_font_glyph_rec(glyphs, min_index, center_index - 1, char_code);
    }
    else if (center_glyph->char_code < char_code) {
        return find_font_glyph_rec(glyphs, center_index + 1, max_index, char_code);
    }
    else {
        return center_glyph;
    }
}

FontGlyphInfo* find_font_glyph(FontGlyphInfo* glyphs, uint32_t glyph_count, uint16_t char_code) {
    return find_font_glyph_rec(glyphs, 0, glyph_count - 1, char_code);
}

int measure_text_width(FontGlyphInfo* glyphs, uint32_t glyph_count, const char* str) {
    FontGlyphInfo* glyph = find_font_glyph(glyphs, glyph_count, str[0]);
    int width = glyph->left;

    do {
        width += glyph->width + CHAR_SPACER;
        str += 1;
        glyph = find_font_glyph(glyphs, glyph_count, str[0]);
    } while (str[0] != '\0');

    return width - CHAR_SPACER;
}