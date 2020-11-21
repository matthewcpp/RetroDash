#include "renderer.h"

#include "filesystem.h"

#include <stdlib.h>

typedef struct {
    int8_t top;
    int8_t left;
    int8_t right;
} FontCharInfo;

struct Font {
    Sprite* sprite;
    FontCharInfo* info;
};

Font* renderer_load_font(Renderer* renderer, const char* sprite_base_path, const char* font_path) {
    int font_info_handle = filesystem_open(font_path);
    if (font_info_handle < 0) {
        return NULL;
    }

    Sprite* sprite = renderer_load_sprite(renderer, sprite_base_path);

    if (!sprite) {
        filesystem_close(font_info_handle);
        return NULL;
    }

    Font* font = malloc(sizeof(Font));
    font->sprite = sprite;

    uint32_t char_count;
    filesystem_read(&char_count, sizeof(uint32_t), 1, font_info_handle);
    font->info = malloc(sizeof(FontCharInfo) * char_count);
    filesystem_read(font->info, sizeof(FontCharInfo), char_count, font_info_handle);
    filesystem_close(font_info_handle);
    return font;
}

void renderer_destroy_font(Renderer* renderer, Font* font) {
    renderer_destroy_sprite(renderer, font->sprite);

    free(font->info);
    free(font);
}

void renderer_draw_text_string(Renderer* renderer, Font* font, const char* str, int x, int y ) {
    while (str[0] != '\0') {
        int frame = str[0] - ' ';
        FontCharInfo* info = font->info + frame;
        renderer_draw_sprite_frame(renderer, font->sprite, frame, x - info->left, y - info->top);
        x += info->right;

        str += 1;
    }
}

int renderer_measure_text_width(Renderer* renderer, Font* font, const char* str) {
    int width = 0;

    while (str[0] != '\0') {
        int frame = str[0] - ' ';
        width += font->info[frame].right;

        str += 1;
    }

    return width;
}
