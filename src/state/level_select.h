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
}LevelInfo;

typedef struct {
    uint32_t count;
    LevelInfo* levels;
    char* data;
} LevelList;

typedef struct {
    Audio* _audio;
    Input* _input;
    Renderer* _renderer;

    Font* _font;
    Music* music;
    Sprite* _title_sprite;
    Sprite* _selector_arrows;
    Sprite* _selector_dots;
    Sprite* _selected_level_name_sprite;

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