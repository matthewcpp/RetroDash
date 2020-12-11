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
    Sprite* _nav_sprite;

    Music* _preview_music;
    PreviewMusicState _preview_music_state;
    float _preview_music_time;

    LevelList _level_list;
    int _selected_level_index;
    Point _screen_size;
    GameState transition;
} LevelSelectScreen;

LevelSelectScreen* level_select_screen_create(Audio* audio, Input* input, Renderer* renderer);
void level_select_screen_destroy(LevelSelectScreen* level_select);

void level_select_screen_update(LevelSelectScreen* level_select, float time_delta);
void level_select_screen_draw(LevelSelectScreen* level_select);

char* level_select_screen_get_selected_path(LevelSelectScreen* level_select);

#endif