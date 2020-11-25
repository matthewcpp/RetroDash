#ifndef PAUSE_DIALOG
#define PAUSE_DIALOG

#include "../input.h"
#include "../renderer.h"

#include "dialog.h"

typedef struct {
    Dialog base;

    Sprite* _paused_title_sprite;
    Sprite* _resume_sprite;
    Sprite* _quit_sprite;
} PauseDialog;

void pause_dialog_init(PauseDialog* dialog, Input* input, Renderer* renderer, Font* title_font, Font* info_font);
void pause_dialog_uninit(PauseDialog* dialog);

void pause_dialog_draw(PauseDialog* dialog);
void pause_dialog_update(PauseDialog* dialog);

void pause_dialog_show(PauseDialog* dialog);
void pause_dialog_hide(PauseDialog* dialog);

#endif