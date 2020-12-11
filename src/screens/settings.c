#include "settings.h"

#include "screen_util.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void settings_init(Settings* settings) {
    settings->player_speed_modifier = 1.0f;
}

static void settings_set_game_speed_sprite(SettingsScreen* settings) {
    if (settings->_game_speed_sprite)
        renderer_destroy_sprite(settings->_renderer, settings->_game_speed_sprite);

    char buffer[8];
    int value = (int)roundf(settings->_settings->player_speed_modifier * 100.0f);
    sprintf(buffer, "%d%%", value);

    settings->_game_speed_sprite = renderer_create_text_sprite(settings->_renderer, settings->_settings_font, buffer);
}

SettingsScreen* settings_screen_create(Renderer* renderer, Input* input, Settings* settings) {
    SettingsScreen* state = malloc(sizeof(SettingsScreen));

    state->_renderer = renderer;
    state->_input = input;
    state->_settings = settings;
    state->transition = GAME_STATE_NONE;

    state->_title_sprite = renderer_load_sprite(state->_renderer, "settings/settings");
    state->_arrows_sprite = renderer_load_sprite(state->_renderer, "settings/arrows");
    Font* nav_font = renderer_load_font(state->_renderer, "common/nav_font");
    state->_nav_sprite = renderer_create_text_sprite(state->_renderer, nav_font, ") ACCEPT");
    renderer_destroy_font(renderer, nav_font);

    state->_settings_font = renderer_load_font(state->_renderer, "dialog/dialog_title_font");

    state->_game_speed_label_sprite = renderer_create_text_sprite(state->_renderer, state->_settings_font, "GAME SPEED:");
    state->_game_speed_sprite =  NULL;
    settings_set_game_speed_sprite(state);

    return state;
}

void settings_screen_destroy(SettingsScreen* settings) {
    renderer_destroy_sprite(settings->_renderer, settings->_title_sprite);
    renderer_destroy_sprite(settings->_renderer, settings->_game_speed_label_sprite);
    renderer_destroy_sprite(settings->_renderer, settings->_game_speed_sprite);
    renderer_destroy_sprite(settings->_renderer, settings->_nav_sprite);
    renderer_destroy_sprite(settings->_renderer, settings->_arrows_sprite);
    renderer_destroy_font(settings->_renderer, settings->_settings_font);

    free(settings);
}

#define SETTINGS_SPEED_MIN 0.5f
#define SETTINGS_SPEED_MAX 1.5
#define SETTINGS_SPEED_STEP 0.1f

void settings_screen_update(SettingsScreen* settings, float time_delta) {
    if (input_button_is_down(settings->_input, CONTROLLER_1, CONTROLLER_BUTTON_START)) {
        settings->transition = GAME_STATE_TITLE;
    }

    if (screen_util_ui_nav_left(settings->_input) && settings->_settings->player_speed_modifier > SETTINGS_SPEED_MIN) {
        settings->_settings->player_speed_modifier -= SETTINGS_SPEED_STEP;
        settings_set_game_speed_sprite(settings);
    }
    else if (screen_util_ui_nav_right(settings->_input) &&  settings->_settings->player_speed_modifier < SETTINGS_SPEED_MAX) {
        settings->_settings->player_speed_modifier += SETTINGS_SPEED_STEP;
        settings_set_game_speed_sprite(settings);
    }
}

#define SETTINGS_GRID_SIZE 32
#define SETTINGS_PADDING 30
#define ARROW_PADDING 16
#define GAME_SPEED_SPRITE_ALLOCATED_SIZE 55
#define NAV_SPRITE_PADDING 6

void settings_screen_draw(SettingsScreen* settings) {
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

    x_pos -= ARROW_PADDING + GAME_SPEED_SPRITE_ALLOCATED_SIZE;
    int offset = (GAME_SPEED_SPRITE_ALLOCATED_SIZE - sprite_width(settings->_game_speed_sprite)) / 2; // center sprite in its allocated space
    renderer_draw_sprite(settings->_renderer, settings->_game_speed_sprite, x_pos + offset, y_pos);

    x_pos -= ARROW_PADDING + arrow_size;
    if (settings->_settings->player_speed_modifier > SETTINGS_SPEED_MIN)
        renderer_draw_sprite_frame(settings->_renderer, settings->_arrows_sprite, 0, x_pos, y_pos + arrow_offset);

    renderer_draw_sprite(settings->_renderer, settings->_nav_sprite, NAV_SPRITE_PADDING, screen_size.y - sprite_height(settings->_nav_sprite) - NAV_SPRITE_PADDING);
}