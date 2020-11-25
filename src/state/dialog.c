#include "dialog.h"

void dialog_init(Dialog* dialog, Input* input, Renderer* renderer){
    dialog->shown = 0;
    dialog->action = DIALOG_ACTION_NONE;
    dialog->_selected_action = DIALOG_ACTION_RETRY;

    dialog->_input = input;
    dialog->_renderer = renderer;
}

void dialog_update(Dialog* dialog) {
    if (input_button_is_down(dialog->_input, CONTROLLER_1, CONTROLLER_BUTTON_A) ||
        input_button_is_down(dialog->_input, CONTROLLER_1, CONTROLLER_BUTTON_START) ) {
        dialog->action = dialog->_selected_action;
    }

    if (dialog->_selected_action == DIALOG_ACTION_RETRY && input_button_is_down(dialog->_input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_RIGHT))
        dialog->_selected_action = DIALOG_ACTION_RETURN;
    else if (dialog->_selected_action == DIALOG_ACTION_RETURN && input_button_is_down(dialog->_input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_LEFT))
        dialog->_selected_action = DIALOG_ACTION_RETRY;
}

void dialog_draw_and_adjust_box(Dialog* dialog, Rect* dialog_rect) {
    renderer_set_color(dialog->_renderer, 37, 236, 236, 255);
    renderer_draw_filled_rect(dialog->_renderer, dialog_rect);

    dialog_rect->x += 2;
    dialog_rect->y += 2;
    dialog_rect->w -= DIALOG_BORDER * 2;
    dialog_rect->h -= DIALOG_BORDER * 2;

    renderer_set_color(dialog->_renderer, 10, 7, 53, 255);
    renderer_draw_filled_rect(dialog->_renderer, dialog_rect);
}

void dialog_draw_options(Dialog* dialog, Rect* dialog_rect, int y_pos, Sprite* sprite_a, Sprite* sprite_b) {
    int x_pos = dialog_rect->x + 20;
    renderer_draw_sprite(dialog->_renderer, sprite_a, x_pos, y_pos);

    int str_b_width = sprite_width(sprite_b);
    x_pos = dialog_rect->x + dialog_rect->w - 20 - str_b_width;
    renderer_draw_sprite(dialog->_renderer, sprite_b, x_pos, y_pos);

    y_pos += 18 + 3;

    Rect indicator_rect;
    indicator_rect.y = y_pos;
    indicator_rect.h = 2;

    if (dialog->_selected_action == DIALOG_ACTION_RETRY) {
        indicator_rect.x = dialog_rect->x + 20;
        indicator_rect.w = sprite_width(sprite_a);
    }
    else {
        indicator_rect.x = dialog_rect->x + dialog_rect->w - 20 - str_b_width;
        indicator_rect.w = str_b_width;
    }

    renderer_set_color(dialog->_renderer, 37, 236, 236, 255);
    renderer_draw_filled_rect(dialog->_renderer, &indicator_rect);
}

void dialog_show(Dialog* dialog) {
    dialog->action = DIALOG_ACTION_NONE;
    dialog->_selected_action = DIALOG_ACTION_RETRY;
    dialog->shown = 1;
}

void dialog_hide(Dialog* dialog) {
    dialog->shown = 0;
}