#include "playing_base.h"

#include <stddef.h>

void playing_screen_base_init(PlayingScreenBase* screen, Audio* audio, Renderer* renderer, Input* input, const char* level_path, const char* info_font_path, GameSettings* settings){
    screen->_audio = audio;
    screen->_input = input;
    screen->_renderer = renderer;
    screen->_paused = 0;

    background_init(&screen->background, renderer);

    Point screen_size;
    renderer_get_screen_size(renderer, &screen_size);

    screen->camera = camera_create(screen_size.x, screen_size.y);
    screen->level = level_create(audio, renderer, screen->camera);

    screen->_title_font = renderer_load_font(renderer, "/dialog/dialog_title_font");
    screen->_info_font = renderer_load_font(renderer, info_font_path);

    level_load(screen->level, level_path);

    screen->player = player_create(screen->level, renderer, screen->camera, input, settings);
    camera_set_target(screen->camera, &screen->player->entity);
    camera_set_safe_margins(screen->camera, -1.5f, 5.0f);
    camera_update(screen->camera);

    teleport_init(&screen->teleport, screen->_renderer);
    screen->teleport_in_hook = NULL;
    screen->teleport_out_hook = NULL;
    screen->hook_user_data = NULL;

    screen->_just_loaded = 1;

    attempt_dialog_init(&screen->_attempt_dialog, input, renderer, screen->player, screen->_title_font, screen->_info_font);
    pause_dialog_init(&screen->_pause_dialog, input, renderer, screen->_title_font, screen->_info_font);
}

void playing_screen_base_uninit(PlayingScreenBase* screen) {
    level_destroy(screen->level);
    camera_destroy(screen->camera);
    player_destroy(screen->player);

    attempt_dialog_uninit(&screen->_attempt_dialog);
    pause_dialog_uninit(&screen->_pause_dialog);

    renderer_destroy_font(screen->_renderer, screen->_title_font);
    renderer_destroy_font(screen->_renderer, screen->_info_font);
}

static void update_player_active(PlayingScreenBase* screen, float time_delta) {
    if (input_button_is_down(screen->_input, CONTROLLER_1, CONTROLLER_BUTTON_START)) {
        audio_pause_music(screen->_audio);
        screen->_paused = 1;
    }

    if (screen->teleport.status == TELEPORT_STATUS_ACTIVE)
        teleport_update(&screen->teleport, time_delta);
    else
        player_update(screen->player, time_delta);

    level_update(screen->level, time_delta);
    camera_update(screen->camera);

    if (screen->player->velocity.x > 0.0f)
        background_update(&screen->background, time_delta);
}

static void update_state_paused(PlayingScreenBase* screen, float time_delta) {
    if (!screen->_pause_dialog.base.shown) {
        pause_dialog_show(&screen->_pause_dialog);
    }
    else {
        pause_dialog_update(&screen->_pause_dialog);
    }

    if (screen->_pause_dialog.base.action == DIALOG_ACTION_RETRY) {
        screen->_paused = 0;
        audio_resume_music(screen->_audio);
        pause_dialog_hide(&screen->_pause_dialog);
    }
    else if (screen->_pause_dialog.base.action == DIALOG_ACTION_RETURN){
        screen->dialog_return_hook(screen->hook_user_data);
    }
}

static void reset_scene(PlayingScreenBase* screen) {
    screen->_attempt_dialog.base.shown = 0;
    background_reset(&screen->background);
    level_reset(screen->player->_level);
    player_reset(screen->player);
    camera_update(screen->camera);
    teleport_in(&screen->teleport, screen->player, screen->camera, screen->teleport_in_hook, screen->hook_user_data);
    audio_restart_music(screen->_audio);
}

static void update_attempt_dialog(PlayingScreenBase* screen) {
    attempt_dialog_update(&screen->_attempt_dialog);

    if (screen->_attempt_dialog.base.action == DIALOG_ACTION_RETURN){
        attempt_dialog_hide(&screen->_attempt_dialog);
        screen->dialog_return_hook(screen->hook_user_data);
    }
    else if (screen->_attempt_dialog.base.action == DIALOG_ACTION_RETRY) {
        attempt_dialog_hide(&screen->_attempt_dialog);
        screen->_attempt_dialog.base.action = DIALOG_ACTION_NONE;
        reset_scene(screen);
    }
}

void playing_screen_base_update(PlayingScreenBase* screen, float time_delta) {
    if (screen->_just_loaded == 1) {
        teleport_in(&screen->teleport, screen->player, screen->camera, screen->teleport_in_hook, screen->hook_user_data);
        audio_play_music(screen->_audio, screen->level->music);
#ifdef EMSCRIPTEN
        audio_restart_music(screen->_audio);
#endif
        screen->_just_loaded = 0;
        return;
    }

    if (screen->_paused) {
        update_state_paused(screen, time_delta);
    }
    else if (screen->_attempt_dialog.base.shown) {
        update_attempt_dialog(screen);
    }
    else if (screen->player->state == PLAYER_STATE_DEAD) {
        attempt_dialog_show(&screen->_attempt_dialog);
        audio_pause_music(screen->_audio);
    }
    else if (screen->player->state == PLAYER_STATE_REACHED_GOAL && screen->teleport.status != TELEPORT_STATUS_ACTIVE) {
        teleport_out(&screen->teleport, screen->player, screen->camera, screen->teleport_out_hook, screen->hook_user_data);
    }
    else {
        update_player_active(screen, time_delta);
    }
}

void playing_screen_base_draw(PlayingScreenBase* screen) {
    background_draw(&screen->background);
    level_draw(screen->level);

    if (screen->teleport.status == TELEPORT_STATUS_ACTIVE)
        teleport_draw(&screen->teleport);
    else if (screen->_attempt_dialog.base.shown)
        attempt_dialog_draw(&screen->_attempt_dialog);

    else if (screen->_pause_dialog.base.shown)
        pause_dialog_draw(&screen->_pause_dialog);
    else
        player_draw(screen->player);
}