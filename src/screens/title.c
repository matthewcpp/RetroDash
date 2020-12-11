#include "title.h"

#include "screen_util.h"

#include <stdlib.h>

#define CHARACTER_SCALE 1.25f

static void create_menu(TitleScreen* title);

TitleScreen* title_screen_create(Audio* audio, Input* input, Renderer* renderer){
    TitleScreen* title = malloc(sizeof(TitleScreen));

    title->_audio = audio;
    title->_input = input;
    title->_renderer = renderer;
    title->transition = GAME_SCREEN_NONE;
    title->menu_selection = TITLE_MENU_START;

    title->_title_sprite = NULL;
    title->_title_sprite = renderer_load_sprite(title->_renderer, "title/title");
    title->_character_top = renderer_load_sprite(title->_renderer, "title/title_idle_top");
    title->_character_bottom = renderer_load_sprite(title->_renderer, "title/title_idle_bottom");
    title->_platform = renderer_load_sprite(title->_renderer, "title/title_platform");
    title->_menu_font = renderer_load_font(title->_renderer, "title/title_menu_font");
    title->_menu_font_selected = renderer_load_font(title->_renderer, "title/title_menu_font_selected");
    create_menu(title);

    title->_music = audio_load_music(title->_audio, "title/title_music");
    audio_play_music(audio, title->_music);
#ifdef EMSCRIPTEN
    audio_restart_music(audio);
#endif

    animation_player_init(&title->_animation);
    animation_player_load(&title->_animation, "title/title_idle.animation");
    title->_animation.frame_time = 1.0f;
    animation_player_set_current(&title->_animation, 0, 1);

    return title;
}

void title_screen_destroy(TitleScreen* title){
    renderer_destroy_sprite(title->_renderer, title->_title_sprite);
    renderer_destroy_sprite(title->_renderer, title->_character_top);
    renderer_destroy_sprite(title->_renderer, title->_character_bottom);
    renderer_destroy_sprite(title->_renderer, title->_platform);
    audio_destroy_music(title->_audio, title->_music);

    for (int i = 0; i < TITLE_MENU_COUNT; i++) {
        renderer_destroy_sprite(title->_renderer, title->_menu_sprite[i]);
        renderer_destroy_sprite(title->_renderer, title->_menu_sprite_selected[i]);
    }

    renderer_destroy_font(title->_renderer, title->_menu_font);
    renderer_destroy_font(title->_renderer, title->_menu_font_selected);
    animation_player_uninit(&title->_animation);
    free(title);
}

void title_screen_update(TitleScreen* title, float time_delta){
    animation_player_update(&title->_animation, time_delta);

    if (screen_util_ui_nav_down(title->_input)) {
        if (title->menu_selection < TITLE_MENU_COUNT - 1)
            title->menu_selection += 1;
    }
    else if (screen_util_ui_nav_up(title->_input)) {
        if (title->menu_selection > 0)
            title->menu_selection -= 1;
    }
    if (screen_util_ui_button_accept(title->_input)){

        switch (title->menu_selection) {
            case TITLE_MENU_START:
                title->transition = GAME_SCREEN_LEVEL_SELECT;
                break;

            case TITLE_MENU_TUTORIAL:
                title->transition = GAME_SCREEN_TUTORIAL;
                break;

            case TITLE_MENU_SETTINGS:
                title->transition = GAME_SCREEN_SETTINGS;
                break;
        }
    }
}

static void draw_character(TitleScreen* title, Point* screen_size) {
    int platform_top = screen_size->y - (int)(sprite_vertical_frame_size(title->_platform) * CHARACTER_SCALE);
    int character_slice_height = (int)(sprite_vertical_frame_size(title->_character_top) * CHARACTER_SCALE);

    renderer_draw_scaled_sprite(title->_renderer, title->_platform,
                                0, platform_top,
                                1.5f, CHARACTER_SCALE,
                                0);

    renderer_draw_scaled_sprite(title->_renderer, title->_character_bottom,
                                25, platform_top - character_slice_height,
                                CHARACTER_SCALE, CHARACTER_SCALE,
                                title->_animation.frame);

    renderer_draw_scaled_sprite(title->_renderer, title->_character_top,
                                25, platform_top - 2 * character_slice_height,
                                CHARACTER_SCALE, CHARACTER_SCALE,
                                title->_animation.frame);
}

static void draw_menu(TitleScreen* title, Point* screen_size) {
    int size = font_size(title->_menu_font);
    int x_pos = screen_size->x - sprite_width(title->_menu_sprite[TITLE_MENU_START]) - 10;
    int y_pos = screen_size->y - TITLE_MENU_COUNT * size - 10;

    for (int i = 0; i < TITLE_MENU_COUNT; i++) {
        renderer_draw_sprite(title->_renderer, title->menu_selection == i ? title->_menu_sprite_selected[i] : title->_menu_sprite[i], x_pos, y_pos);
        y_pos += size;
    }
}

#define TITLE_SCREEN_GRID_SIZE 32

void title_screen_draw(TitleScreen* title){
    Point screen_size;
    renderer_get_screen_size(title->_renderer, &screen_size);

    renderer_set_color(title->_renderer, 33, 7, 58, 255);
    renderer_draw_grid(title->_renderer, 0, TITLE_SCREEN_GRID_SIZE);

    int title_x = (screen_size.x / 2) - (sprite_width(title->_title_sprite) / 2);
    int title_y = 10;

    renderer_draw_sprite(title->_renderer, title->_title_sprite, title_x, title_y);

    draw_character(title, &screen_size);
    draw_menu(title, &screen_size);
}

void create_menu(TitleScreen* title) {
    title->_menu_sprite[0] = renderer_create_text_sprite(title->_renderer, title->_menu_font, "START GAME");
    title->_menu_sprite_selected[0] = renderer_create_text_sprite(title->_renderer, title->_menu_font_selected, "START GAME");

    title->_menu_sprite[1] = renderer_create_text_sprite(title->_renderer, title->_menu_font, "TUTORIAL");
    title->_menu_sprite_selected[1] = renderer_create_text_sprite(title->_renderer, title->_menu_font_selected, "TUTORIAL");

    title->_menu_sprite[2] = renderer_create_text_sprite(title->_renderer, title->_menu_font, "SETTINGS");
    title->_menu_sprite_selected[2] = renderer_create_text_sprite(title->_renderer, title->_menu_font_selected, "SETTINGS");
}
