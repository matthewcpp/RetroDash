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
    TITLE_MENU_SETTINGS,
    TITLE_MENU_COUNT
} TitleMenuItem;

typedef struct {
    Audio* _audio;
    Input* _input;
    Renderer* _renderer;
    GameScreen transition;

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
} TitleScreen;

TitleScreen* title_screen_create(Audio* audio, Input* input, Renderer* renderer);
void title_screen_destroy(TitleScreen* title);

void title_screen_update(TitleScreen* title, float time_delta);
void title_screen_draw(TitleScreen* title);

#endif