#ifndef ATTEMPT_DIALOG
#define ATTEMPT_DIALOG

#include "../input.h"
#include "../player.h"
#include "../renderer.h"

typedef enum {
    ACTION_NONE,
    ACTION_RETRY,
    ACTION_RETURN
} DialogAction;

typedef struct {
    DialogAction action;

    DialogAction _selected_action;
    int shown;

    Font* _title_font;
    Font* _info_font;

    char _level_title_text[25];
    char _attempt_text[13];
    int _level_title_width;
    int _attempt_width;

    Input* _input;
    Renderer* _renderer;
    Player* _player;
}AttemptDialog;

void attempt_dialog_init(AttemptDialog* attempt_dialog, Input* input, Renderer* renderer, Player* player);
void attempt_dialog_uninit(AttemptDialog* attempt_dialog);

void attempt_dialog_show(AttemptDialog* attempt_dialog);

void attempt_dialog_update(AttemptDialog* attempt_dialog, float time_delta);
void attempt_dialog_draw(AttemptDialog* attempt_dialog);

#endif