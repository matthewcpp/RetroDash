#ifndef ATTEMPT_DIALOG
#define ATTEMPT_DIALOG

#include "../input.h"
#include "../player.h"
#include "../renderer.h"

typedef enum {
    DIALOG_ACTION_NONE,
    DIALOG_ACTION_RETRY,
    DIALOG_ACTION_RETURN
} DialogAction;

typedef struct {
    DialogAction action;

    DialogAction _selected_action;
    int shown;

    Font* _title_font;
    Font* _info_font;

    char _level_title_text[25];
    char _attempt_text[14]; // 9999 attempts
    char _percent_text[5];
    char _jump_text[12]; // 9999 jumps

    int _level_title_width;
    int _attempt_width;
    int _percent_width;
    int _jump_width;

    int _retry_width;
    int _return_width;

    Input* _input;
    Renderer* _renderer;
    Player* _player;
}AttemptDialog;

void attempt_dialog_init(AttemptDialog* dialog, Input* input, Renderer* renderer, Player* player);
void attempt_dialog_uninit(AttemptDialog* dialog);

void attempt_dialog_show(AttemptDialog* dialog);

void attempt_dialog_update(AttemptDialog* dialog, float time_delta);
void attempt_dialog_draw(AttemptDialog* dialog);

#endif