#ifndef SETTINGS
#define SETTINGS

#include "../input.h"
#include "../renderer.h"
#include "states.h"

typedef struct {
    float player_speed_modifier;
} Settings;

void settings_init(Settings* settings);

typedef struct {
    Renderer* _renderer;
    Input* _input;
    Settings* _settings;
    Sprite* _title_sprite;
    Sprite* _arrows_sprite;
    Sprite* _game_speed_label_sprite;
    Sprite* _game_speed_sprite;
    Sprite* _nav_sprite;
    Font* _settings_font;
    GameState transition;
} SettingsScreen;

SettingsScreen* settings_screen_create(Renderer* renderer, Input* input, Settings* settings);
void settings_screen_destroy(SettingsScreen* settings);

void settings_screen_update(SettingsScreen* settings, float time_delta);
void settings_screen_draw(SettingsScreen* settings);


#endif