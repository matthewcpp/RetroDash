#include "level_select.h"

#include "../filesystem.h"

#include <stdlib.h>

static void set_selected_level(StateLevelSelect* level_select, int index) {
    if (level_select->music)
        audio_destroy_music(level_select->_audio, level_select->music);

    level_select->music = audio_load_music(level_select->_audio, level_select->_level_list.levels[index].music);
    audio_play_music(level_select->_audio, level_select->music);

    level_select->_selected_level_index = index;
}

static void load_level_list(StateLevelSelect* level_select) {
    int level_list_handle = filesystem_open("/level_list");

    uint32_t payload_size;
    filesystem_read(&payload_size, sizeof(uint32_t), 1, level_list_handle);
    level_select->_level_list.data = malloc(payload_size);
    filesystem_read(level_select->_level_list.data , 1, payload_size, level_list_handle);

    filesystem_read(&level_select->_level_list.count, sizeof(uint32_t), 1, level_list_handle);
    level_select->_level_list.levels = malloc(level_select->_level_list.count * sizeof(LevelInfo));

    for (uint32_t i = 0; i < level_select->_level_list.count; i++) {
        LevelInfo* info = level_select->_level_list.levels + i;
        uint32_t indices[3];
        filesystem_read(indices , sizeof(uint32_t), 3, level_list_handle);

        info->name = level_select->_level_list.data + indices[0];
        info->path = level_select->_level_list.data + indices[1];
        info->music = level_select->_level_list.data + indices[2];
    }

    filesystem_close(level_list_handle);
}

StateLevelSelect* state_level_select_create(Audio* audio, Input* input, Renderer* renderer){
    StateLevelSelect* level_select = malloc(sizeof(StateLevelSelect));

    level_select->_audio = audio;
    level_select->_input = input;
    level_select->_renderer = renderer;
    level_select->transition = GAME_STATE_NONE;

    level_select->_font = renderer_load_font(renderer, "/Basic_LAZER", "/Basic_LAZER.font");

    level_select->_title_sprite = renderer_load_sprite(level_select->_renderer, "/select_level");
    level_select->_selector_arrows = renderer_load_sprite(level_select->_renderer, "/selector_arrows");
    level_select->_selector_dots = renderer_load_sprite(level_select->_renderer, "/selector_dots");
    level_select->music = NULL;

    renderer_get_screen_size(level_select->_renderer, &level_select->_screen_size);
    load_level_list(level_select);

    set_selected_level(level_select,0);

    return level_select;
}

void state_level_select_destroy(StateLevelSelect* level_select) {
    audio_destroy_music(level_select->_audio, level_select->music);
    renderer_destroy_sprite(level_select->_renderer, level_select->_title_sprite);
    renderer_destroy_sprite(level_select->_renderer, level_select->_selector_arrows);
    renderer_destroy_sprite(level_select->_renderer, level_select->_selector_dots);
    renderer_destroy_font(level_select->_renderer, level_select->_font);

    free(level_select->_level_list.data);
    free(level_select->_level_list.levels);

    free(level_select);
}

void state_level_select_update(StateLevelSelect* level_select, float time_delta){
    if (input_button_is_down(level_select->_input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_RIGHT)) {
        if (level_select->_selected_level_index < level_select->_level_list.count - 1)
            set_selected_level(level_select, level_select->_selected_level_index + 1);
    }

    if (input_button_is_down(level_select->_input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_LEFT)) {
        if (level_select->_selected_level_index > 0)
            set_selected_level(level_select, level_select->_selected_level_index - 1);
    }

    if (input_button_is_down(level_select->_input, CONTROLLER_1, CONTROLLER_BUTTON_A) ||
        input_button_is_down(level_select->_input, CONTROLLER_1, CONTROLLER_BUTTON_START) ) {
        level_select->transition = GAME_STATE_PLAYING;
    }
}

static void draw_selected_level_info(StateLevelSelect* level_select) {
    const char* level_text = level_select->_level_list.levels[level_select->_selected_level_index].name;
    int width = renderer_measure_text_width(level_select->_renderer, level_select->_font, level_text);

    renderer_draw_text_string(level_select->_renderer, level_select->_font, level_text, (level_select->_screen_size.x / 2) - (width / 2), level_select->_screen_size.y / 2 - 15);
}

static void draw_selector_arrows(StateLevelSelect* level_select) {
    int selector_arrow_height = sprite_height(level_select->_selector_arrows);

    if (level_select->_selected_level_index > 0) {
        renderer_draw_sprite_frame(level_select->_renderer, level_select->_selector_arrows, 0, 24, level_select->_screen_size.y / 2 - selector_arrow_height / 2);
    }

    if (level_select->_selected_level_index < level_select->_level_list.count - 1) {
        renderer_draw_sprite_frame(level_select->_renderer, level_select->_selector_arrows, 1, level_select->_screen_size.x - 24 - sprite_horizontal_frame_size(level_select->_selector_arrows), level_select->_screen_size.y / 2 - selector_arrow_height / 2);
    }
}

#define SELECTOR_DOT_SPACER 5

static void draw_selector_dots(StateLevelSelect* level_select) {
    int dot_width = sprite_horizontal_frame_size(level_select->_selector_dots);

    int total_width = dot_width * level_select->_level_list.count + SELECTOR_DOT_SPACER * (level_select->_level_list.count - 1);

    int draw_x = level_select->_screen_size.x / 2 - total_width / 2;
    int draw_y = level_select->_screen_size.y - 24 - sprite_vertical_frame_size(level_select->_selector_dots);

    for (int i = 0; i < level_select->_level_list.count; i++) {
        int frame = i == level_select->_selected_level_index ? 0 : 1;
        renderer_draw_sprite_frame(level_select->_renderer, level_select->_selector_dots, frame, draw_x, draw_y);

        draw_x += dot_width + SELECTOR_DOT_SPACER;
    }
}

void state_level_select_draw(StateLevelSelect* level_select) {
    renderer_set_color(level_select->_renderer, 33, 7, 58, 255);
    renderer_draw_grid(level_select->_renderer);

    int title_width = sprite_width(level_select->_title_sprite);
    renderer_draw_sprite(level_select->_renderer, level_select->_title_sprite, (level_select->_screen_size.x / 2) - (title_width / 2), 10);

    draw_selected_level_info(level_select);
    draw_selector_arrows(level_select);
    draw_selector_dots(level_select);
}

char* state_level_select_get_selected_path(StateLevelSelect* level_select) {
    return level_select->_level_list.levels[level_select->_selected_level_index].path;
}