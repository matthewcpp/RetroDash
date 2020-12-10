#ifndef ATTEMPT_DIALOG
#define ATTEMPT_DIALOG

#include "../player.h"

#include "dialog.h"

typedef struct {
    Dialog base;
    Player* _player;

    Font* _title_font;
    Font* _info_font;

    Sprite* _level_title_sprite;
    Sprite* _attempt_count_sprite;
    Sprite* _percent_complete_sprite;
    Sprite* _jump_count_sprite;
    Sprite* _size_changes_sprite;
    Sprite* _retry_sprite;
    Sprite* _return_sprite;
} AttemptDialog;

void attempt_dialog_init(AttemptDialog* dialog, Input* input, Renderer* renderer, Player* player, Font* title_font, Font* info_font);
void attempt_dialog_uninit(AttemptDialog* dialog);

void attempt_dialog_show(AttemptDialog* dialog);
void attempt_dialog_hide(AttemptDialog* dialog);

void attempt_dialog_update(AttemptDialog* dialog);
void attempt_dialog_draw(AttemptDialog* dialog);

#endif