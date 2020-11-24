#ifndef DIALOG
#define DIALOG

#include "../input.h"
#include "../renderer.h"

#define DIALOG_BORDER 2

typedef enum {
    DIALOG_ACTION_NONE,
    DIALOG_ACTION_RETRY,
    DIALOG_ACTION_RETURN
} DialogAction;

typedef struct {
    DialogAction action;
    DialogAction _selected_action;
    int shown;

    Input* _input;
    Renderer* _renderer;
} Dialog;

void dialog_init(Dialog* dialog, Input* input, Renderer* renderer);
void dialog_update(Dialog* dialog);

void dialog_draw_and_adjust_box(Dialog* dialog, Rect* rect);
void dialog_draw_options(Dialog* dialog, Rect* dialog_rect, int y_pos, Font* font, const char* str_a, int str_a_width, const char* str_b, int str_b_width);

void dialog_show(Dialog* dialog);
void dialog_hide(Dialog* dialog);

#endif