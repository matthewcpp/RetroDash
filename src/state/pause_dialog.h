#ifndef PAUSE_DIALOG
#define PAUSE_DIALOG

#include "../input.h"
#include "../renderer.h"

#include "dialog.h"

typedef struct {
    Dialog base;

    Input* _input;
    Renderer* _renderer;

    Font* _title_font;
    Font* _info_font;

    int _pause_width;
    int _resume_width;
    int _quit_width;
} PauseDialog;

void pause_dialog_init(PauseDialog* dialog, Input* input, Renderer* renderer, Font* title_font, Font* info_font);
void pause_dialog_draw(PauseDialog* dialog);
void pause_dialog_update(PauseDialog* dialog);

void pause_dialog_show(PauseDialog* dialog);
void pause_dialog_hide(PauseDialog* dialog);

#endif