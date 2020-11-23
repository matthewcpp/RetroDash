#ifndef PLAYING_H
#define PLAYING_H

#include "../audio.h"
#include "../camera.h"
#include "../level.h"
#include "../player.h"

#include "attempt_dialog.h"
#include "pause_dialog.h"
#include "states.h"

typedef struct {
    Camera* camera;
    Level* level;
    Player* player;

    AttemptDialog _attempt_dialog;
    PauseDialog _pause_dialog;
    Input* _input;
    Audio* _audio;
    Renderer* _renderer;
    int _paused;

    Font* _title_font;
    Font* _info_font;

    /** N64: its possible that loading the audio can take some amount of time.  Since we cant do this on another thread it can cause the time delta to spike from the previous frame. */
    int _just_loaded;
    GameState transition;
} StatePlaying;

StatePlaying* state_playing_create(Audio* audio, Renderer* renderer, Input* input, const char* level_path);
void state_playing_destroy(StatePlaying* state);

void state_playing_update(StatePlaying* state, float time_delta);
void state_playing_draw(StatePlaying* state);

#endif