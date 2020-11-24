#include "pause_dialog.h"

void pause_dialog_init(PauseDialog* dialog, Input* input, Renderer* renderer, Font* title_font, Font* info_font) {
    dialog_init(&dialog->base, input, renderer);

    dialog->_title_font = title_font;
    dialog->_info_font = info_font;

    dialog->_pause_width = renderer_measure_text_width(dialog->_renderer, dialog->_title_font, "PAUSED");
    dialog->_resume_width = renderer_measure_text_width(dialog->_renderer, dialog->_info_font, "RESUME");
    dialog->_quit_width = renderer_measure_text_width(dialog->_renderer, dialog->_info_font, "QUIT");
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
    int x_pos = dialog_rect.x + dialog_rect.w / 2 - dialog->_pause_width / 2;

    renderer_draw_text_string(dialog->base._renderer, dialog->_title_font, "PAUSED", x_pos, y_pos);
    y_pos += 24 + 5;

    dialog_draw_options(&dialog->base, &dialog_rect, y_pos, dialog->_info_font, "RESUME", dialog->_resume_width, "QUIT", dialog->_quit_width);
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
