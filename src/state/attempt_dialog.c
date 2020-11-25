#include "attempt_dialog.h"

#include <stdio.h>
#include <string.h>

void attempt_dialog_init(AttemptDialog* dialog, Input* input, Renderer* renderer, Player* player, Font* title_font, Font* info_font) {
    dialog_init(&dialog->base, input, renderer);
    
    dialog->_player = player;

    dialog->_title_font = title_font;
    dialog->_info_font = info_font;

    dialog->_return_sprite = renderer_create_text_sprite(dialog->base._renderer, dialog->_info_font, "RETURN");
    dialog->_retry_sprite = renderer_create_text_sprite(dialog->base._renderer, dialog->_info_font, "RETRY");
    dialog->_level_title_sprite = renderer_create_text_sprite(dialog->base._renderer, dialog->_title_font, dialog->_player->_level->name);

    dialog->_attempt_count_sprite = NULL;
}

static void clear_results_sprites(AttemptDialog* dialog) {
    if (dialog->_attempt_count_sprite != NULL) {
        renderer_destroy_sprite(dialog->base._renderer, dialog->_attempt_count_sprite);
        renderer_destroy_sprite(dialog->base._renderer, dialog->_percent_complete_sprite);
        renderer_destroy_sprite(dialog->base._renderer, dialog->_jump_count_sprite);
    }
}

void attempt_dialog_uninit(AttemptDialog* dialog) {
    renderer_destroy_sprite(dialog->base._renderer, dialog->_level_title_sprite);
    renderer_destroy_sprite(dialog->base._renderer, dialog->_retry_sprite);
    renderer_destroy_sprite(dialog->base._renderer, dialog->_return_sprite);
    clear_results_sprites(dialog);
}

void compute_results_sprites(AttemptDialog* dialog) {
    char str_buffer[64];

    clear_results_sprites(dialog);

    sprintf(str_buffer, "ATTEMPT %d", dialog->_player->attempt_count);
    dialog->_attempt_count_sprite = renderer_create_text_sprite(dialog->base._renderer, dialog->_info_font, str_buffer);

    int percent_complete = (int)((dialog->_player->distance_travelled / level_travel_distance(dialog->_player->_level)) * 100.0f);
    sprintf(str_buffer, "%d%%", percent_complete);
    dialog->_percent_complete_sprite = renderer_create_text_sprite(dialog->base._renderer, dialog->_info_font, str_buffer);

    sprintf(str_buffer, "JUMPS: %d", dialog->_player->jump_count);
    dialog->_jump_count_sprite = renderer_create_text_sprite(dialog->base._renderer, dialog->_info_font, str_buffer);
}

void attempt_dialog_show(AttemptDialog* dialog) {
    compute_results_sprites(dialog);
    dialog_show(&dialog->base);

}

void attempt_dialog_update(AttemptDialog* dialog){
    dialog_update(&dialog->base);
}

#define DIALOG_WIDTH 220
#define DIALOG_HEIGHT 164

#define PROGRESS_BAR_WIDTH 200
#define PROGRESS_BAR_HEIGHT 18

void attempt_dialog_draw(AttemptDialog* dialog) {
    Point screen_size;
    renderer_get_screen_size(dialog->base._renderer, &screen_size);

    Rect dialog_rect;
    dialog_rect.x = screen_size.x / 2 - DIALOG_WIDTH / 2;
    dialog_rect.y = screen_size.y / 2 - DIALOG_HEIGHT / 2;
    dialog_rect.w = DIALOG_WIDTH;
    dialog_rect.h = DIALOG_HEIGHT;

    dialog_draw_and_adjust_box(&dialog->base, &dialog_rect);

    renderer_set_color(dialog->base._renderer, 255, 255, 255, 255);
    int x_pos = dialog_rect.x + dialog_rect.w / 2 - sprite_width(dialog->_level_title_sprite) / 2;
    int y_pos = dialog_rect.y + 5;

    renderer_draw_sprite(dialog->base._renderer, dialog->_level_title_sprite, x_pos, y_pos);
    y_pos += 24 + 5;

    x_pos = dialog_rect.x + dialog_rect.w / 2 - sprite_width(dialog->_attempt_count_sprite) / 2;
    renderer_draw_sprite(dialog->base._renderer, dialog->_attempt_count_sprite, x_pos, y_pos);
    y_pos += 18 + 5;

    Rect progress_bar;
    progress_bar.x = dialog_rect.x + DIALOG_WIDTH / 2 - PROGRESS_BAR_WIDTH / 2;
    progress_bar.y = y_pos;
    progress_bar.w = PROGRESS_BAR_WIDTH;
    progress_bar.h = PROGRESS_BAR_HEIGHT;

    renderer_set_color(dialog->base._renderer, 10, 7, 40, 255);
    renderer_draw_filled_rect(dialog->base._renderer, &progress_bar);

    float dist_percent = dialog->_player->distance_travelled / level_travel_distance(dialog->_player->_level);
    progress_bar.w = (int)(dist_percent * PROGRESS_BAR_WIDTH);

    renderer_set_color(dialog->base._renderer, 117, 251, 76, 255);
    renderer_draw_filled_rect(dialog->base._renderer, &progress_bar);

    x_pos = dialog_rect.x + dialog_rect.w / 2 - sprite_width(dialog->_percent_complete_sprite) / 2;
    y_pos = progress_bar.y;

    renderer_draw_sprite(dialog->base._renderer, dialog->_percent_complete_sprite, x_pos, y_pos);

    y_pos += PROGRESS_BAR_HEIGHT + 10;
    x_pos = dialog_rect.x + dialog_rect.w / 2 - sprite_width(dialog->_jump_count_sprite) / 2;

    renderer_draw_sprite(dialog->base._renderer, dialog->_jump_count_sprite, x_pos, y_pos);

    y_pos += 18 + 15;

    dialog_draw_options(&dialog->base, &dialog_rect, y_pos, dialog->_retry_sprite, dialog->_return_sprite);
}

void attempt_dialog_hide(AttemptDialog* dialog) {
    dialog_hide(&dialog->base);
}
