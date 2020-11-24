#ifndef ATTEMPT_DIALOG
#define ATTEMPT_DIALOG

#include "../player.h"

#include "dialog.h"

typedef struct {
    Dialog base;

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


    Player* _player;
}AttemptDialog;

void attempt_dialog_init(AttemptDialog* dialog, Input* input, Renderer* renderer, Player* player, Font* title_font, Font* info_font);

void attempt_dialog_show(AttemptDialog* dialog);
void attempt_dialog_hide(AttemptDialog* dialog);

void attempt_dialog_update(AttemptDialog* dialog);
void attempt_dialog_draw(AttemptDialog* dialog);

#endif