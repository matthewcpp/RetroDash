#ifndef FONT_PRIVATE
#define FONT_PRIVATE

#include <stdint.h>

#define CHAR_SPACER 0

typedef struct {
    uint16_t char_code;

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

FontGlyphInfo* find_font_glyph(FontGlyphInfo* glyphs, uint32_t glyph_count, uint16_t char_code);
int measure_text_width(FontGlyphInfo* glyphs, uint32_t glyph_count, const char* str);

#endif