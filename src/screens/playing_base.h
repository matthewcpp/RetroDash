#ifndef PLAYING_BASE
#define PLAYING_BASE

#include "../audio.h"
#include "../background.h"
#include "../camera.h"
#include "../level.h"
#include "../player.h"
#include "../teleport.h"

#include "attempt_dialog.h"
#include "pause_dialog.h"
#include "states.h"

typedef struct {
    Background background;
    Camera* camera;
    Level* level;
    Player* player;
    Teleport teleport;

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

    TeleportCallback teleport_in_hook;
    TeleportCallback teleport_out_hook;
    TeleportCallback dialog_return_hook;
    void* hook_user_data;
} StatePlayingBase;

void state_playing_base_init(StatePlayingBase* state, Audio* audio, Renderer* renderer, Input* input, const char* level_path, const char* info_font_path);
void state_playing_base_uninit(StatePlayingBase* state);

void state_playing_base_update(StatePlayingBase* state, float time_delta);
void state_playing_base_draw(StatePlayingBase* state);

#endif