#ifndef PLAYING_H
#define PLAYING_H

#include "../audio.h"
#include "../camera.h"
#include "../level.h"
#include "../player.h"
#include "../teleport.h"

#include "playing_base.h"
#include "settings.h"
#include "states.h"

typedef struct {
    PlayingScreenBase base;
    GameScreen transition;
} PlayingScreen;

PlayingScreen* playing_screen_create(Audio* audio, Renderer* renderer, Input* input, const char* level_path, GameSettings* settings);
void playing_screen_destroy(PlayingScreen* playing_screen);

void playing_screen_update(PlayingScreen* playing_screen, float time_delta);
void playing_screen_draw(PlayingScreen* playing_screen);

#endif