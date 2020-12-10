#include "level_select.h"

#include "../filesystem.h"

#include <stdlib.h>

typedef enum {
    LEVEL_DIFFICULTY_EASY,
    LEVEL_DIFFICULTY_MEDIUM,
    LEVEL_DIFFICULTY_HARD
} LevelDifficulty;

static void set_selected_level(StateLevelSelect* level_select, int index) {
    if (index == level_select->_selected_level_index)
        return;

    if (level_select->_preview_music)
        audio_destroy_music(level_select->_audio, level_select->_preview_music);

    if (level_select->_selected_level_name_sprite) {
        renderer_destroy_sprite(level_select->_renderer, level_select->_selected_level_name_sprite);
    }

    if (level_select->_selected_level_difficulty_sprite) {
        renderer_destroy_sprite(level_select->_renderer, level_select->_selected_level_difficulty_sprite);
    }

    level_select->_selected_level_name_sprite = renderer_create_text_sprite(level_select->_renderer, level_select->_level_title_font, level_select->_level_list.levels[index].name);
    level_select->_preview_music = audio_load_music(level_select->_audio, level_select->_level_list.levels[index].music);

    switch (level_select->_level_list.levels[index].difficulty) {
        case LEVEL_DIFFICULTY_EASY:
            level_select->_selected_level_difficulty_sprite = renderer_create_text_sprite(level_select->_renderer, level_select->_level_info_font, "DIFFICULTY: EASY");
            break;

        case LEVEL_DIFFICULTY_MEDIUM:
            level_select->_selected_level_difficulty_sprite = renderer_create_text_sprite(level_select->_renderer, level_select->_level_info_font, "DIFFICULTY: MEDIUM");
            break;

        case LEVEL_DIFFICULTY_HARD:
            level_select->_selected_level_difficulty_sprite = renderer_create_text_sprite(level_select->_renderer, level_select->_level_info_font, "DIFFICULTY: HARD");
            break;
    }

    level_select->_preview_music_state = PREVIEW_MUSIC_WAITING;
    level_select->_preview_music_time = 0.0f;

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

        filesystem_read(&info->difficulty , sizeof(uint32_t), 1, level_list_handle);
    }

    filesystem_close(level_list_handle);
}

StateLevelSelect* state_level_select_create(Audio* audio, Input* input, Renderer* renderer){
    StateLevelSelect* level_select = malloc(sizeof(StateLevelSelect));

    level_select->_audio = audio;
    level_select->_input = input;
    level_select->_renderer = renderer;
    level_select->transition = GAME_STATE_NONE;

    level_select->_level_title_font = renderer_load_font(renderer, "level_select/level_select_font");
    level_select->_level_info_font = renderer_load_font(renderer, "level_select/level_select_info_font");
    Font* nav_font = renderer_load_font(renderer, "common/nav_font");
    level_select->_nav_sprite = renderer_create_text_sprite(renderer, nav_font, "( RETURN");
    renderer_destroy_font(renderer, nav_font);

    level_select->_title_sprite = renderer_load_sprite(level_select->_renderer, "level_select/select_level");
    level_select->_selector_arrows = renderer_load_sprite(level_select->_renderer, "level_select/selector_arrows");
    level_select->_selector_dots = renderer_load_sprite(level_select->_renderer, "level_select/selector_dots");
    level_select->_preview_music = NULL;
    level_select->_selected_level_name_sprite = NULL;

    renderer_get_screen_size(level_select->_renderer, &level_select->_screen_size);
    load_level_list(level_select);

    level_select->_selected_level_index = -1;
    set_selected_level(level_select,0);

    return level_select;
}

void state_level_select_destroy(StateLevelSelect* level_select) {
    audio_set_music_volume(level_select->_audio, 1.0f); // TODO: create a transition_out method or something for things like this?
    audio_destroy_music(level_select->_audio, level_select->_preview_music);

    renderer_destroy_sprite(level_select->_renderer, level_select->_title_sprite);
    renderer_destroy_sprite(level_select->_renderer, level_select->_selector_arrows);
    renderer_destroy_sprite(level_select->_renderer, level_select->_selector_dots);
    renderer_destroy_sprite(level_select->_renderer, level_select->_nav_sprite);

    renderer_destroy_font(level_select->_renderer, level_select->_level_title_font);
    renderer_destroy_font(level_select->_renderer, level_select->_level_info_font);

    if (level_select->_selected_level_name_sprite)
        renderer_destroy_sprite(level_select->_renderer, level_select->_selected_level_name_sprite);

    if (level_select->_selected_level_difficulty_sprite)
        renderer_destroy_sprite(level_select->_renderer, level_select->_selected_level_difficulty_sprite);

    free(level_select->_level_list.data);
    free(level_select->_level_list.levels);

    free(level_select);
}

#define PREVIEW_MUSIC_WAITING_TIME 0.75f
#define PREVIEW_MUSIC_FADE_TIME 1.0f
#define PREVIEW_MUSIC_PLAYING_TIME 10.0f

static void _update_preview_music(StateLevelSelect* level_select, float time_delta) {
    level_select->_preview_music_time += time_delta;

    switch (level_select->_preview_music_state) {
        case PREVIEW_MUSIC_WAITING:
            if (level_select->_preview_music_time >= PREVIEW_MUSIC_WAITING_TIME) {
                audio_set_music_volume(level_select->_audio, 0.0f);
                audio_play_music(level_select->_audio, level_select->_preview_music);
                #ifdef EMSCRIPTEN
                    audio_restart_music(level_select->_audio);
                #endif

                level_select->_preview_music_time = 0.0f;
                level_select->_preview_music_state = PREVIEW_MUSIC_FADE_IN;
            }
            break;

        case PREVIEW_MUSIC_FADE_IN:
            if (level_select->_preview_music_time >= PREVIEW_MUSIC_FADE_TIME) {
                audio_set_music_volume(level_select->_audio, 1.0f);
                level_select->_preview_music_time = 0.0f;
                level_select->_preview_music_state = PREVIEW_MUSIC_PLAYING;
            }
            else {
                audio_set_music_volume(level_select->_audio, level_select->_preview_music_time / PREVIEW_MUSIC_FADE_TIME);
            }
            break;

        case PREVIEW_MUSIC_PLAYING:
            if (level_select->_preview_music_time >= PREVIEW_MUSIC_PLAYING_TIME) {
                level_select->_preview_music_time = 0.0f;
                level_select->_preview_music_state = PREVIEW_MUSIC_FADE_OUT;
            }
            break;

        case PREVIEW_MUSIC_FADE_OUT:
            if (level_select->_preview_music_time >= PREVIEW_MUSIC_FADE_TIME) {
                level_select->_preview_music_time = 0.0f;
                level_select->_preview_music_state = PREVIEW_MUSIC_DONE;
            }
            else {
                audio_set_music_volume(level_select->_audio, 1.0f - (level_select->_preview_music_time / PREVIEW_MUSIC_FADE_TIME));
            }
            break;

        case PREVIEW_MUSIC_DONE:
            break;
    }
}

void state_level_select_update(StateLevelSelect* level_select, float time_delta){
    if (input_button_is_down(level_select->_input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_RIGHT) ||
        input_button_is_down(level_select->_input, CONTROLLER_1, CONTROLLER_BUTTON_C_RIGHT)) {
        if (level_select->_selected_level_index < level_select->_level_list.count - 1)
            set_selected_level(level_select, level_select->_selected_level_index + 1);
    }

    if (input_button_is_down(level_select->_input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_LEFT) ||
        input_button_is_down(level_select->_input, CONTROLLER_1, CONTROLLER_BUTTON_C_LEFT)) {
        if (level_select->_selected_level_index > 0)
            set_selected_level(level_select, level_select->_selected_level_index - 1);
    }

    if (input_button_is_down(level_select->_input, CONTROLLER_1, CONTROLLER_BUTTON_A) ||
        input_button_is_down(level_select->_input, CONTROLLER_1, CONTROLLER_BUTTON_Z) ||
        input_button_is_down(level_select->_input, CONTROLLER_1, CONTROLLER_BUTTON_START) ) {
        level_select->transition = GAME_STATE_PLAYING;
    }

    if (input_button_is_down(level_select->_input, CONTROLLER_1, CONTROLLER_BUTTON_B)) {
        level_select->transition = GAME_STATE_TITLE;
    }

    _update_preview_music(level_select, time_delta);
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

#define LEVEL_SELECT_GRID_SIZE 32
#define LEVEL_SELECT_NAV_PADDING 5

void state_level_select_draw(StateLevelSelect* level_select) {
    renderer_set_color(level_select->_renderer, 33, 7, 58, 255);
    renderer_draw_grid(level_select->_renderer, 0, LEVEL_SELECT_GRID_SIZE);

    int title_width = sprite_width(level_select->_title_sprite);
    renderer_draw_sprite(level_select->_renderer, level_select->_title_sprite, (level_select->_screen_size.x / 2) - (title_width / 2), 10);

    int y_pos = level_select->_screen_size.y / 2 - 25;
    int x_pos = (level_select->_screen_size.x / 2) - (sprite_width(level_select->_selected_level_name_sprite) / 2);
    renderer_draw_sprite(level_select->_renderer, level_select->_selected_level_name_sprite, x_pos, y_pos);

    y_pos += font_size(level_select->_level_title_font) + 5;
    x_pos = (level_select->_screen_size.x / 2) - (sprite_width(level_select->_selected_level_difficulty_sprite) / 2);
    renderer_draw_sprite(level_select->_renderer, level_select->_selected_level_difficulty_sprite, x_pos, y_pos);

    draw_selector_arrows(level_select);
    draw_selector_dots(level_select);

    renderer_draw_sprite(level_select->_renderer, level_select->_nav_sprite, LEVEL_SELECT_NAV_PADDING, level_select->_screen_size.y - sprite_height(level_select->_nav_sprite) - LEVEL_SELECT_NAV_PADDING);
}

char* state_level_select_get_selected_path(StateLevelSelect* level_select) {
    return level_select->_level_list.levels[level_select->_selected_level_index].path;
}