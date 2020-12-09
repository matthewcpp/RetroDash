#ifndef LEVEL_SELECT_H
#define LEVEL_SELECT_H

#include "../audio.h"
#include "../input.h"
#include "../renderer.h"
#include "states.h"

typedef struct {
    char* name;
    char* path;
    char* music;
    uint32_t difficulty;
}LevelInfo;

typedef struct {
    uint32_t count;
    LevelInfo* levels;
    char* data;
} LevelList;

typedef enum {
    PREVIEW_MUSIC_WAITING,
    PREVIEW_MUSIC_FADE_IN,
    PREVIEW_MUSIC_PLAYING,
    PREVIEW_MUSIC_FADE_OUT,
    PREVIEW_MUSIC_DONE
} PreviewMusicState;

typedef struct {
    Audio* _audio;
    Input* _input;
    Renderer* _renderer;

    Font* _level_title_font;
    Font* _level_info_font;

    Sprite* _title_sprite;
    Sprite* _selector_arrows;
    Sprite* _selector_dots;
    Sprite* _selected_level_name_sprite;
    Sprite* _selected_level_difficulty_sprite;

    Music* _preview_music;
    PreviewMusicState _preview_music_state;
    float _preview_music_time;

    LevelList _level_list;
    int _selected_level_index;
    Point _screen_size;
    GameState transition;
} StateLevelSelect;

StateLevelSelect* state_level_select_create(Audio* audio, Input* input, Renderer* renderer);
void state_level_select_destroy(StateLevelSelect* level_select);

void state_level_select_update(StateLevelSelect* level_select, float time_delta);
void state_level_select_draw(StateLevelSelect* level_select);

char* state_level_select_get_selected_path(StateLevelSelect* level_select);

#endif