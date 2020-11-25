#include "pause_dialog.h"

void pause_dialog_init(PauseDialog* dialog, Input* input, Renderer* renderer, Font* title_font, Font* info_font) {
    dialog_init(&dialog->base, input, renderer);

    dialog->_paused_title_sprite = renderer_create_text_sprite(renderer, title_font, "PAUSED");
    dialog->_resume_sprite = renderer_create_text_sprite(renderer, info_font, "RESUME");
    dialog->_quit_sprite = renderer_create_text_sprite(renderer, info_font, "QUIT");
}

void pause_dialog_uninit(PauseDialog* dialog) {
    renderer_destroy_sprite(dialog->base._renderer, dialog->_paused_title_sprite);
    renderer_destroy_sprite(dialog->base._renderer, dialog->_resume_sprite);
    renderer_destroy_sprite(dialog->base._renderer, dialog->_quit_sprite);
}

#define DIALOG_WIDTH 160
#define DIALOG_HEIGHT 100

void pause_dialog_draw(PauseDialog* dialog) {
    Point screen_size;
    renderer_get_screen_size(dialog->base._renderer, &screen_size);

    Rect dialog_rect;
    dialog_rect.x = screen_size.x / 2 - DIALOG_WIDTH / 2;
    dialog_rect.y = screen_size.y / 2 - DIALOG_HEIGHT / 2;
    dialog_rect.w = DIALOG_WIDTH;
    dialog_rect.h = DIALOG_HEIGHT;

    dialog_draw_and_adjust_box(&dialog->base, &dialog_rect);

    int y_pos = dialog_rect.y + 5;
    int x_pos = dialog_rect.x + dialog_rect.w / 2 - sprite_width(dialog->_paused_title_sprite) / 2;

    renderer_draw_sprite(dialog->base._renderer, dialog->_paused_title_sprite, x_pos, y_pos);
    y_pos += 24 + 5;

    dialog_draw_options(&dialog->base, &dialog_rect, y_pos, dialog->_resume_sprite, dialog->_quit_sprite);
}

void pause_dialog_update(PauseDialog* dialog) {
    dialog_update(&dialog->base);
}

void pause_dialog_show(PauseDialog* dialog) {
    dialog_show(&dialog->base);
}

void pause_dialog_hide(PauseDialog* dialog) {
    dialog_hide(&dialog->base);
}
