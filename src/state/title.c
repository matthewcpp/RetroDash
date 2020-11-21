#include "title.h"

#include <stdlib.h>

typedef enum{
    TITLE_MENU_START,
    TITLE_MENU_CREDITS,
    TITLE_MENU_COUNT
} TitleMenuItem;

#define START_SPRITE_INDEX 0
#define CREDIT_SPRITE_INDEX 2

#define CHARACTER_SCALE 1.25f

StateTitle* state_title_create(Audio* audio, Input* input, Renderer* renderer){
    StateTitle* state = malloc(sizeof(StateTitle));

    state->_audio = audio;
    state->_input = input;
    state->_renderer = renderer;
    state->transition = GAME_STATE_NONE;
    state->menu_selection = TITLE_MENU_START;

    state->_title_sprite = NULL;
    state->_title_sprite = renderer_load_sprite(state->_renderer, "/title");
    state->_character_top = renderer_load_sprite(state->_renderer, "/title_idle_top");
    state->_character_bottom = renderer_load_sprite(state->_renderer, "/title_idle_bottom");
    state->_platform = renderer_load_sprite(state->_renderer, "/title_platform");
    state->_menu = renderer_load_sprite(state->_renderer, "/title_menu");
    state->_music = audio_load_music(state->_audio, "/title_music.mod");

    if (state->_music == NULL)
        renderer_set_clear_color(state->_renderer, 255, 0, 0);
    else
        audio_play_music(audio, state->_music);

    animation_player_init(&state->_animation);
    animation_player_load(&state->_animation, "/title_idle.animation");
    state->_animation.frame_time = 1.0f;
    animation_player_set_current(&state->_animation, 0, 1);



    return state;
}

void state_title_destroy(StateTitle* state){
    renderer_destroy_sprite(state->_renderer, state->_title_sprite);
    renderer_destroy_sprite(state->_renderer, state->_character_top);
    renderer_destroy_sprite(state->_renderer, state->_character_bottom);
    renderer_destroy_sprite(state->_renderer, state->_platform);
    audio_destroy_music(state->_audio, state->_music);

    free(state);
}

void state_title_update(StateTitle* state, float time_delta){
    animation_player_update(&state->_animation, time_delta);

    if (input_button_is_down(state->_input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_DOWN)) {
        if (state->menu_selection < TITLE_MENU_COUNT - 1)
            state->menu_selection += 1;
    }
    else if (input_button_is_down(state->_input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_UP)) {
        if (state->menu_selection > 0)
            state->menu_selection -= 1;
    }
    if (input_button_is_down(state->_input, CONTROLLER_1, CONTROLLER_BUTTON_START) ||
        input_button_is_down(state->_input, CONTROLLER_1, CONTROLLER_BUTTON_A)){
        if (state->menu_selection == TITLE_MENU_START)
            state->transition = GAME_STATE_LEVEL_SELECT;
    }
}

static void draw_character(StateTitle* state, Point* screen_size) {
    int platform_top = screen_size->y - (int)(sprite_vertical_frame_size(state->_platform) * CHARACTER_SCALE);
    int character_slice_height = (int)(sprite_vertical_frame_size(state->_character_top) * CHARACTER_SCALE);

    renderer_draw_scaled_sprite(state->_renderer, state->_platform,
                                0, platform_top,
                                1.5f, CHARACTER_SCALE,
                                0);

    renderer_draw_scaled_sprite(state->_renderer, state->_character_bottom,
                                25, platform_top - character_slice_height,
                                CHARACTER_SCALE, CHARACTER_SCALE,
                                state->_animation.frame);

    renderer_draw_scaled_sprite(state->_renderer, state->_character_top,
                                25, platform_top - 2 * character_slice_height,
                                CHARACTER_SCALE, CHARACTER_SCALE,
                                state->_animation.frame);
}

static void draw_menu(StateTitle* state, Point* screen_size) {
    int height = sprite_vertical_frame_size(state->_menu);

    int y_pos = screen_size->y - height - 30;
    int x_pos = screen_size->x - sprite_width(state->_menu) - 10;

    int row = state->menu_selection == TITLE_MENU_CREDITS ? CREDIT_SPRITE_INDEX + 1 : CREDIT_SPRITE_INDEX;
    renderer_draw_sprite_row(state->_renderer, state->_menu, row, x_pos, y_pos);

    y_pos -= 2 * height;
    row = state->menu_selection == TITLE_MENU_START ? START_SPRITE_INDEX + 1 : START_SPRITE_INDEX;
    renderer_draw_sprite_row(state->_renderer, state->_menu, row, x_pos, y_pos);
}

void state_title_draw(StateTitle* state){
    Point screen_size;
    renderer_get_screen_size(state->_renderer, &screen_size);

    renderer_set_color(state->_renderer, 33, 7, 58, 255);
    renderer_draw_grid(state->_renderer);

    int title_x = (screen_size.x / 2) - (sprite_width(state->_title_sprite) / 2);
    int title_y = 10;

    renderer_draw_sprite(state->_renderer, state->_title_sprite, title_x, title_y);

    draw_character(state, &screen_size);
    draw_menu(state, &screen_size);
}