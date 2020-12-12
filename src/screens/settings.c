#include "settings.h"

#include "screen_util.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void settings_set_game_speed_sprite(SettingsScreen* settings) {
    if (settings->_game_speed_sprite)
        renderer_destroy_sprite(settings->_renderer, settings->_game_speed_sprite);

    char buffer[8];
    int value = (int)roundf(settings->_settings->player_speed_modifier * 100.0f);
    sprintf(buffer, "%d%%", value);

    settings->_game_speed_sprite = renderer_create_text_sprite(settings->_renderer, settings->_settings_font, buffer);
}

static void settings_set_enable_checkpoints_sprite(SettingsScreen* settings) {
    if (settings->_enable_checkpoints_sprite)
        renderer_destroy_sprite(settings->_renderer, settings->_enable_checkpoints_sprite);

    char buffer[8];
    if (settings->_settings->enable_checkpoints)
        strcpy(buffer, "ON");
    else
        strcpy(buffer, "OFF");

    settings->_enable_checkpoints_sprite = renderer_create_text_sprite(settings->_renderer, settings->_settings_font, buffer);
}

SettingsScreen* settings_screen_create(Renderer* renderer, Input* input, GameSettings* settings) {
    SettingsScreen* state = malloc(sizeof(SettingsScreen));

    state->_renderer = renderer;
    state->_input = input;
    state->_settings = settings;
    state->transition = GAME_SCREEN_NONE;

    state->_title_sprite = renderer_load_sprite(state->_renderer, "settings/settings");
    state->_arrows_sprite = renderer_load_sprite(state->_renderer, "settings/arrows");
    Font* nav_font = renderer_load_font(state->_renderer, "common/nav_font");
    state->_nav_sprite = renderer_create_text_sprite(state->_renderer, nav_font, ") ACCEPT");
    renderer_destroy_font(renderer, nav_font);

    state->_settings_font = renderer_load_font(state->_renderer, "dialog/dialog_title_font");

    state->_game_speed_label_sprite = renderer_create_text_sprite(state->_renderer, state->_settings_font, "GAME SPEED:");
    state->_enable_checkpoints_label_sprite = renderer_create_text_sprite(state->_renderer, state->_settings_font, "CHECKPOINTS:");
    state->_active_item = SETTINGS_MENU_SPEED;

    state->_game_speed_sprite =  NULL;
    settings_set_game_speed_sprite(state);

    state->_enable_checkpoints_sprite = NULL;
    settings_set_enable_checkpoints_sprite(state);

    return state;
}

void settings_screen_destroy(SettingsScreen* settings) {
    renderer_destroy_sprite(settings->_renderer, settings->_title_sprite);
    renderer_destroy_sprite(settings->_renderer, settings->_game_speed_label_sprite);
    renderer_destroy_sprite(settings->_renderer, settings->_game_speed_sprite);
    renderer_destroy_sprite(settings->_renderer, settings->_enable_checkpoints_label_sprite);
    renderer_destroy_sprite(settings->_renderer, settings->_enable_checkpoints_sprite);
    renderer_destroy_sprite(settings->_renderer, settings->_nav_sprite);
    renderer_destroy_sprite(settings->_renderer, settings->_arrows_sprite);
    renderer_destroy_font(settings->_renderer, settings->_settings_font);

    free(settings);
}

#define SETTINGS_SPEED_MIN 0.5f
#define SETTINGS_SPEED_MAX 1.5
#define SETTINGS_SPEED_STEP 0.1f

static void settings_screen_update_speed(SettingsScreen* settings) {
    if (screen_util_ui_nav_left(settings->_input) && settings->_settings->player_speed_modifier > SETTINGS_SPEED_MIN) {
        settings->_settings->player_speed_modifier -= SETTINGS_SPEED_STEP;
        settings_set_game_speed_sprite(settings);
    }
    else if (screen_util_ui_nav_right(settings->_input) &&  settings->_settings->player_speed_modifier < SETTINGS_SPEED_MAX) {
        settings->_settings->player_speed_modifier += SETTINGS_SPEED_STEP;
        settings_set_game_speed_sprite(settings);
    }
}

static void settings_screen_update_checkpoints(SettingsScreen* settings) {
    if ((screen_util_ui_nav_left(settings->_input) && settings->_settings->enable_checkpoints)) {
        settings->_settings->enable_checkpoints = 0;
        settings_set_enable_checkpoints_sprite(settings);
    }
    else if ((screen_util_ui_nav_right(settings->_input) && !settings->_settings->enable_checkpoints)) {
        settings->_settings->enable_checkpoints = 1;
        settings_set_enable_checkpoints_sprite(settings);
    }
}

void settings_screen_update(SettingsScreen* settings, float time_delta) {
    if (input_button_is_down(settings->_input, CONTROLLER_1, CONTROLLER_BUTTON_START)) {
        settings->transition = GAME_SCREEN_TITLE;
    }

    if (screen_util_ui_nav_up(settings->_input) && settings->_active_item > 0) {
        settings->_active_item -= 1;
    }
    else if (screen_util_ui_nav_down(settings->_input) && settings->_active_item < SETTINGS_MENU_ITEM_COUNT - 1) {
        settings->_active_item += 1;
    }

    switch (settings->_active_item) {
        case SETTINGS_MENU_SPEED:
            settings_screen_update_speed(settings);
            break;

        case SETTINGS_MENU_CHECKPOINTS:
            settings_screen_update_checkpoints(settings);
            break;
    }
}

#define INDICATOR_BOX_SIZE 10
#define INDICATOR_BOX_PADDING 7

#define SETTINGS_GRID_SIZE 32
#define SETTINGS_PADDING 30
#define ARROW_PADDING 16
#define GAME_SPEED_SPRITE_ALLOCATED_SIZE 55
#define NAV_SPRITE_PADDING 6

static void draw_setting(SettingsScreen* settings, Sprite* label, Sprite* value, int x_pos, int y_pos, int left_arrow, int right_arrow, int draw_indicator) {
    Point screen_size;
    renderer_get_screen_size(settings->_renderer, &screen_size);

    if (draw_indicator) {
        Rect indicator_rect;
        indicator_rect.x = x_pos - INDICATOR_BOX_PADDING - INDICATOR_BOX_SIZE;
        indicator_rect.y = y_pos + (sprite_height(label) - INDICATOR_BOX_SIZE)  / 2;
        indicator_rect.w = INDICATOR_BOX_SIZE;
        indicator_rect.h = INDICATOR_BOX_SIZE;

        renderer_set_color(settings->_renderer, 37, 236, 236, 255);
        renderer_draw_filled_rect(settings->_renderer, &indicator_rect);
    }

    renderer_draw_sprite(settings->_renderer, label, x_pos, y_pos);

    int arrow_size = sprite_horizontal_frame_size(settings->_arrows_sprite);
    int arrow_offset = (font_size(settings->_settings_font) - sprite_vertical_frame_size(settings->_arrows_sprite)) / 2;
    x_pos = screen_size.x - SETTINGS_PADDING - arrow_size;
    if (right_arrow)
        renderer_draw_sprite_frame(settings->_renderer, settings->_arrows_sprite, 1, x_pos, y_pos + arrow_offset);

    x_pos -= ARROW_PADDING + GAME_SPEED_SPRITE_ALLOCATED_SIZE;
    int offset = (GAME_SPEED_SPRITE_ALLOCATED_SIZE - sprite_width(value)) / 2; // center sprite in its allocated space
    renderer_draw_sprite(settings->_renderer, value, x_pos + offset, y_pos);

    x_pos -= ARROW_PADDING + arrow_size;
    if (left_arrow)
        renderer_draw_sprite_frame(settings->_renderer, settings->_arrows_sprite, 0, x_pos, y_pos + arrow_offset);
}

void settings_screen_draw(SettingsScreen* settings) {
    renderer_set_color(settings->_renderer, 33, 7, 58, 255);
    renderer_draw_grid(settings->_renderer, 0, SETTINGS_GRID_SIZE);

    Point screen_size;
    renderer_get_screen_size(settings->_renderer, &screen_size);

    int title_width = sprite_width(settings->_title_sprite);
    renderer_draw_sprite(settings->_renderer, settings->_title_sprite, (screen_size.x / 2) - (title_width / 2), 10);

    int y_pos = screen_size.y / 2 - font_size(settings->_settings_font) / 2;
    int x_pos = SETTINGS_PADDING;
    int left_arrow = settings->_settings->player_speed_modifier > SETTINGS_SPEED_MIN;
    int right_arrow = settings->_settings->player_speed_modifier < SETTINGS_SPEED_MAX;
    int draw_indicator = settings->_active_item == SETTINGS_MENU_SPEED;

    draw_setting(settings, settings->_game_speed_label_sprite, settings->_game_speed_sprite, x_pos, y_pos, left_arrow, right_arrow, draw_indicator);

    y_pos += font_size(settings->_settings_font) + 10;
    x_pos = SETTINGS_PADDING;
    left_arrow = settings->_settings->enable_checkpoints;
    right_arrow = !left_arrow;
    draw_indicator = settings->_active_item == SETTINGS_MENU_CHECKPOINTS;

    draw_setting(settings, settings->_enable_checkpoints_label_sprite, settings->_enable_checkpoints_sprite, x_pos, y_pos, left_arrow, right_arrow, draw_indicator);


    renderer_draw_sprite(settings->_renderer, settings->_nav_sprite, NAV_SPRITE_PADDING, screen_size.y - sprite_height(settings->_nav_sprite) - NAV_SPRITE_PADDING);
}