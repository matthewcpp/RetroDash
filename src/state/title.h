#ifndef TITLE_H
#define TITLE_H

#include "../animation_player.h"
#include "../audio.h"
#include "../input.h"
#include "../renderer.h"
#include "states.h"

typedef enum{
    TITLE_MENU_START,
    TITLE_MENU_TUTORIAL,
    TITLE_MENU_COUNT
} TitleMenuItem;

typedef struct {
    Audio* _audio;
    Input* _input;
    Renderer* _renderer;
    GameState transition;

    Sprite* _title_sprite;
    Sprite* _character_top;
    Sprite* _character_bottom;
    Sprite* _platform;
    Font* _menu_font;
    Font* _menu_font_selected;
    Sprite* _menu_sprite[TITLE_MENU_COUNT];
    Sprite* _menu_sprite_selected[TITLE_MENU_COUNT];
    Music* _music;
    AnimationPlayer _animation;
    int menu_selection;
} StateTitle;

StateTitle* state_title_create(Audio* audio, Input* input, Renderer* renderer);
void state_title_destroy(StateTitle* title);

void state_title_update(StateTitle* title, float time_delta);
void state_title_draw(StateTitle* title);

#endif