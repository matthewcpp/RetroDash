#include "settings.h"

#include <stdio.h>
#include <stdlib.h>

void settings_init(Settings* settings) {
    settings->player_speed_modifier = 1.0f;
}

static void settings_set_game_speed_sprite(StateSettings* settings) {
    if (settings->_game_speed_sprite)
        renderer_destroy_sprite(settings->_renderer, settings->_game_speed_sprite);

    char buffer[8];
    int value = (int)(settings->_settings->player_speed_modifier * 100.0f);
    sprintf(buffer, "%d%%", value);

    settings->_game_speed_sprite = renderer_create_text_sprite(settings->_renderer, settings->_settings_font, buffer);
}

StateSettings* state_settings_create(Renderer* renderer, Input* input, Settings* settings) {
    StateSettings* state = malloc(sizeof(StateSettings));

    state->_renderer = renderer;
    state->_input = input;
    state->_settings = settings;
    state->transition = GAME_STATE_NONE;

    state->_title_sprite = renderer_load_sprite(state->_renderer, "settings/settings");
    state->_arrows_sprite = renderer_load_sprite(state->_renderer, "settings/arrows");
    state->_settings_font = renderer_load_font(state->_renderer, "dialog/dialog_title_font");

    state->_game_speed_label_sprite = renderer_create_text_sprite(state->_renderer, state->_settings_font, "GAME SPEED:");
    state->_game_speed_sprite =  NULL;
    settings_set_game_speed_sprite(state);

    return state;
}

void state_settings_destroy(StateSettings* settings) {
    renderer_destroy_sprite(settings->_renderer, settings->_title_sprite);
    renderer_destroy_sprite(settings->_renderer, settings->_game_speed_label_sprite);
    renderer_destroy_font(settings->_renderer, settings->_settings_font);

    if (settings->_game_speed_sprite)
        renderer_destroy_sprite(settings->_renderer, settings->_game_speed_sprite);

    renderer_destroy_sprite(settings->_renderer, settings->_arrows_sprite);

    free(settings);
}

#define SETTINGS_SPEED_MIN 0.5f
#define SETTINGS_SPEED_MAX 1.5
#define SETTINGS_SPEED_STEP 0.1f

void state_settings_update(StateSettings* settings, float time_delta) {
    if (input_button_is_down(settings->_input, CONTROLLER_1, CONTROLLER_BUTTON_B) ||
        input_button_is_down(settings->_input, CONTROLLER_1, CONTROLLER_BUTTON_L)) {
        settings->transition = GAME_STATE_TITLE;
    }

    if (input_button_is_down(settings->_input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_LEFT) && settings->_settings->player_speed_modifier > SETTINGS_SPEED_MIN) {
        settings->_settings->player_speed_modifier -= SETTINGS_SPEED_STEP;
        settings_set_game_speed_sprite(settings);
    }
    else if (input_button_is_down(settings->_input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_RIGHT) &&  settings->_settings->player_speed_modifier < SETTINGS_SPEED_MAX) {
        settings->_settings->player_speed_modifier += SETTINGS_SPEED_STEP;
        settings_set_game_speed_sprite(settings);
    }
}

#define SETTINGS_GRID_SIZE 32
#define SETTINGS_PADDING 30
#define ARROW_PADDING 16

void state_settings_draw(StateSettings* settings) {
    renderer_set_color(settings->_renderer, 33, 7, 58, 255);
    renderer_draw_grid(settings->_renderer, 0, SETTINGS_GRID_SIZE);

    Point screen_size;
    renderer_get_screen_size(settings->_renderer, &screen_size);

    int title_width = sprite_width(settings->_title_sprite);
    renderer_draw_sprite(settings->_renderer, settings->_title_sprite, (screen_size.x / 2) - (title_width / 2), 10);

    int y_pos = screen_size.y / 2 - font_size(settings->_settings_font) / 2;
    int x_pos = SETTINGS_PADDING;

    renderer_draw_sprite(settings->_renderer, settings->_game_speed_label_sprite, x_pos, y_pos);

    int arrow_size = sprite_horizontal_frame_size(settings->_arrows_sprite);
    int arrow_offset = (font_size(settings->_settings_font) - sprite_vertical_frame_size(settings->_arrows_sprite)) / 2;
    x_pos = screen_size.x - SETTINGS_PADDING - arrow_size;
    if (settings->_settings->player_speed_modifier < SETTINGS_SPEED_MAX)
        renderer_draw_sprite_frame(settings->_renderer, settings->_arrows_sprite, 1, x_pos, y_pos + arrow_offset);

    x_pos -= ARROW_PADDING + sprite_width(settings->_game_speed_sprite);
    renderer_draw_sprite(settings->_renderer, settings->_game_speed_sprite, x_pos, y_pos);

    x_pos -= ARROW_PADDING + arrow_size;
    if (settings->_settings->player_speed_modifier > SETTINGS_SPEED_MIN)
        renderer_draw_sprite_frame(settings->_renderer, settings->_arrows_sprite, 0, x_pos, y_pos + arrow_offset);
}