#include "attempt_dialog.h"

#include <stdio.h>
#include <string.h>

void attempt_dialog_init(AttemptDialog* attempt_dialog, Input* input, Renderer* renderer, Player* player) {
    attempt_dialog->_input = input;
    attempt_dialog->_renderer = renderer;
    attempt_dialog->_player = player;

    attempt_dialog->shown = 0;
    attempt_dialog->action = ACTION_NONE;
    attempt_dialog->_selected_action = ACTION_RETRY;

    attempt_dialog->_title_font = renderer_load_font(renderer, "/dialog_title_font", "/dialog_title_font.font");
    attempt_dialog->_info_font = renderer_load_font(renderer, "/dialog_info_font", "/dialog_info_font.font");
}

void attempt_dialog_uninit(AttemptDialog* attempt_dialog) {
    renderer_destroy_font(attempt_dialog->_renderer, attempt_dialog->_title_font);
    renderer_destroy_font(attempt_dialog->_renderer, attempt_dialog->_info_font);
}

void compute_text_metrics(AttemptDialog* attempt_dialog) {
    strcpy(attempt_dialog->_level_title_text, attempt_dialog->_player->_level->name);
    attempt_dialog->_level_title_width = renderer_measure_text_width(attempt_dialog->_renderer, attempt_dialog->_title_font, attempt_dialog->_level_title_text);

    sprintf(attempt_dialog->_attempt_text, "ATTEMPT %d", attempt_dialog->_player->attempt_count);
    attempt_dialog->_attempt_width = renderer_measure_text_width(attempt_dialog->_renderer, attempt_dialog->_info_font, attempt_dialog->_attempt_text);
}

void attempt_dialog_show(AttemptDialog* attempt_dialog) {
    compute_text_metrics(attempt_dialog);

    attempt_dialog->action = ACTION_NONE;
    attempt_dialog->_selected_action = ACTION_RETRY;
    attempt_dialog->shown = 1;
}

void attempt_dialog_update(AttemptDialog* attempt_dialog, float time_delta){
    if (input_button_is_down(attempt_dialog->_input, CONTROLLER_1, CONTROLLER_BUTTON_A) ||
            input_button_is_down(attempt_dialog->_input, CONTROLLER_1, CONTROLLER_BUTTON_A) ) {
        attempt_dialog->action = attempt_dialog->_selected_action;
    }

    if (attempt_dialog->_selected_action == ACTION_RETRY && input_button_is_down(attempt_dialog->_input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_RIGHT))
        attempt_dialog->_selected_action = ACTION_RETURN;
    else if (attempt_dialog->_selected_action == ACTION_RETURN && input_button_is_down(attempt_dialog->_input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_LEFT))
        attempt_dialog->_selected_action = ACTION_RETRY;

}

#define DIALOG_WIDTH 220
#define DIALOG_HEIGHT 190
#define DIALOG_BORDER 2

#define PROGRESS_BAR_WIDTH 200
#define PROGRESS_BAR_HEIGHT 18

void attempt_dialog_draw(AttemptDialog* attempt_dialog) {
    Point screen_size;
    renderer_get_screen_size(attempt_dialog->_renderer, &screen_size);

    Rect dialog;
    dialog.x = screen_size.x / 2 - DIALOG_WIDTH / 2;
    dialog.y = screen_size.y / 2 - DIALOG_HEIGHT / 2;
    dialog.w = DIALOG_WIDTH;
    dialog.h = DIALOG_HEIGHT;

    renderer_set_color(attempt_dialog->_renderer, 37, 236, 236, 255);
    renderer_draw_filled_rect(attempt_dialog->_renderer, &dialog);

    dialog.x += 2;
    dialog.y += 2;
    dialog.w -= DIALOG_BORDER * 2;
    dialog.h -= DIALOG_BORDER * 2;

    renderer_set_color(attempt_dialog->_renderer, 10, 7, 53, 255);
    renderer_draw_filled_rect(attempt_dialog->_renderer, &dialog);

    renderer_set_color(attempt_dialog->_renderer, 255, 255, 255, 255);
    int x_pos = dialog.x + dialog.w / 2 - attempt_dialog->_level_title_width / 2;
    int y_pos = dialog.y + 5;

    renderer_draw_text_string(attempt_dialog->_renderer, attempt_dialog->_title_font, attempt_dialog->_level_title_text, x_pos, y_pos);
    y_pos += 24;

    x_pos = dialog.x + dialog.w / 2 - attempt_dialog->_attempt_width / 2;
    renderer_draw_text_string(attempt_dialog->_renderer, attempt_dialog->_info_font, attempt_dialog->_attempt_text, x_pos, y_pos);
    y_pos += 18;

    Rect progress_bar;
    progress_bar.x = dialog.x + DIALOG_WIDTH / 2 - PROGRESS_BAR_WIDTH / 2;
    progress_bar.y = y_pos;
    progress_bar.w = PROGRESS_BAR_WIDTH;
    progress_bar.h = PROGRESS_BAR_HEIGHT;

    renderer_set_color(attempt_dialog->_renderer, 10, 7, 40, 255);
    renderer_draw_filled_rect(attempt_dialog->_renderer, &progress_bar);

    float dist_percent = attempt_dialog->_player->distance_travelled / attempt_dialog->_player->_level->goal_dist;
    progress_bar.w = (int)(dist_percent * PROGRESS_BAR_WIDTH);

    renderer_set_color(attempt_dialog->_renderer, 117, 251, 76, 255);
    renderer_draw_filled_rect(attempt_dialog->_renderer, &progress_bar);

}