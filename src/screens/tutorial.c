#include "tutorial.h"

#include <stdlib.h>

static void on_teleport_in_complete(void* user_data);
static void on_teleport_out_complete(void* user_data);
static void on_dialog_return(void* user_data);

typedef struct {
    float pos_x;
    char* hint;
} PracticeHint;

#define PRACTICE_HINT_COUNT 4
static PracticeHint practice_hints[PRACTICE_HINT_COUNT] = {
    {153.0f, "GO LARGE TO MAKE THE JUMP"},
    {164.0f, "GO MEDIUM TO DODGE THE SPIKES"},
    {181.0f, "GO SMALL MID JUMP TO LAND IN TUNNEL"},
    {208.0f, "GO BIG TO SMASH BRICKS"}
};

TutorialScreen* tutorial_screen_create(Audio* audio, Input* input, Renderer* renderer) {
    TutorialScreen* tutorial = malloc(sizeof(TutorialScreen));

    playing_screen_base_init(&tutorial->base, audio, renderer, input, "/tutorial/tutorial.level",
                             "/tutorial/tutorial_info_font");

    tutorial->transition = GAME_STATE_NONE;
    tutorial->phase = TUTORIAL_PHASE_WAITING_FOR_TELEPORT_IN;
    tutorial->base.teleport_in_hook = on_teleport_in_complete;
    tutorial->base.teleport_out_hook = on_teleport_out_complete;
    tutorial->base.dialog_return_hook = on_dialog_return;
    tutorial->base.hook_user_data = tutorial;
    tutorial->practice_hint_index = 0;

    for (int i = 0; i < INFO_TEXT_LINE_COUNT; i++)
        tutorial->_info_text[i] = NULL;

    tutorial->base.player->process_input = 0;

    return tutorial;
}

static void clear_info_texts(TutorialScreen* tutorial) {
    for (int i = 0; i < INFO_TEXT_LINE_COUNT; i++) {
        if (tutorial->_info_text[i]) {
            renderer_destroy_sprite(tutorial->base._renderer, tutorial->_info_text[i]);
            tutorial->_info_text[i] = NULL;
        }
    }
}

void tutorial_screen_destroy(TutorialScreen* tutorial){
    clear_info_texts(tutorial);
    playing_screen_base_uninit(&tutorial->base);
    free(tutorial);
}

static void update_phase_info(TutorialScreen* tutorial) {
    if (input_button_is_down(tutorial->base._input, CONTROLLER_1, CONTROLLER_BUTTON_START)) {
        tutorial->phase = TUTORIAL_STATE_BASIC_MOVEMENT;
        clear_info_texts(tutorial);
        tutorial->_info_text[0] = renderer_create_text_sprite(tutorial->base._renderer, tutorial->base._info_font, "YOUR CHARACTER WILL CONTINUOUSLY");
        tutorial->_info_text[1] = renderer_create_text_sprite(tutorial->base._renderer, tutorial->base._info_font, "MOVE FORWARD");
        player_start(tutorial->base.player);
    }
}

static void update_phase_basic_movement(TutorialScreen* tutorial, float time_delta) {
    playing_screen_base_update(&tutorial->base, time_delta);

    if (tutorial->base.player->entity.position.x >= 32.0f) {
        clear_info_texts(tutorial);
        tutorial->_info_text[0] = renderer_create_text_sprite(tutorial->base._renderer, tutorial->base._info_font, "AVOID OBSTACLES IN YOUR PATH");
        tutorial->_info_text[1] = renderer_create_text_sprite(tutorial->base._renderer, tutorial->base._info_font, "JUMP WITH # OR $");
        tutorial->phase = TUTORIAL_STATE_JUMP;
    }
}

static void update_phase_jump(TutorialScreen* tutorial, float time_delta) {
    if (input_button_is_down(tutorial->base._input, CONTROLLER_1, CONTROLLER_BUTTON_A) || input_button_is_down(tutorial->base._input, CONTROLLER_1, CONTROLLER_BUTTON_Z) ) {
        clear_info_texts(tutorial);
        player_try_jump(tutorial->base.player);
        tutorial->phase = TUTORIAL_PHASE_RUN_TO_LARGE_CHANGE;
    }
}

static void update_phase_run_to_jump2(TutorialScreen* tutorial, float time_delta) {
    playing_screen_base_update(&tutorial->base, time_delta);

    if (tutorial->base.player->entity.position.x >= 40.0f) {
        clear_info_texts(tutorial);
        tutorial->_info_text[0] = renderer_create_text_sprite(tutorial->base._renderer, tutorial->base._info_font, "LARGE SIZE INCREASES JUMP HEIGHT");
        tutorial->_info_text[1] = renderer_create_text_sprite(tutorial->base._renderer, tutorial->base._info_font, "CHANGE TO LARGE SIZE WITH ' OR &");
        tutorial->phase = TUTORIAL_PHASE_CHANGE_TO_LARGE;
    }
}

static void update_phase_change_to_large(TutorialScreen* tutorial, float time_delta) {
    Player* player = tutorial->base.player;


    if (player->current_size == PLAYER_SIZE_LARGE && player->_animation.current_animation != player->_animation.animations + PLAYER_ANIMATION_CHANGE_SIZE) {
        playing_screen_base_update(&tutorial->base, time_delta);
        player->velocity.x = PLAYER_SPEED;

        if (player->entity.position.x >= 42.0f) {
            clear_info_texts(tutorial);
            tutorial->_info_text[0] = renderer_create_text_sprite(tutorial->base._renderer, tutorial->base._info_font, "CLEAR THE OBSTACLE");
            tutorial->phase = TUTORIAL_PHASE_CLEAR_LARGE_OBSTACLE;
        }
    }
    else if (player->state == PLAYER_STATE_CHANGING_SIZE) {
        playing_screen_base_update(&tutorial->base, time_delta);
    }
    else if (input_button_is_down(tutorial->base._input, CONTROLLER_1, CONTROLLER_BUTTON_C_RIGHT) || input_button_is_down(tutorial->base._input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_RIGHT) ) {
        tutorial->base.player->velocity.x = 0;
        player_try_set_size(tutorial->base.player, PLAYER_SIZE_LARGE);
    }
}

static void update_phase_clear_large_obstacle(TutorialScreen* tutorial, float time_delta) {
    if (input_button_is_down(tutorial->base._input, CONTROLLER_1, CONTROLLER_BUTTON_A) || input_button_is_down(tutorial->base._input, CONTROLLER_1, CONTROLLER_BUTTON_Z) ) {
        player_try_jump(tutorial->base.player);
        clear_info_texts(tutorial);
        tutorial->phase = TUTORIAL_PHASE_SMASH_BRICKS;
    }
}

static void update_phase_smash_bricks(TutorialScreen* tutorial, float time_delta) {
    playing_screen_base_update(&tutorial->base, time_delta);
    Player* player = tutorial->base.player;

    if (player->prev_pos.x < 55.0f && player->entity.position.x >= 55.0f) {
        clear_info_texts(tutorial);
        tutorial->_info_text[0] = renderer_create_text_sprite(tutorial->base._renderer, tutorial->base._info_font, "YOU CAN SMASH BRICKS WHILE LARGE");
    }

    if (player->entity.position.x >= 75.0f) {
        clear_info_texts(tutorial);
        tutorial->_info_text[0] = renderer_create_text_sprite(tutorial->base._renderer, tutorial->base._info_font, "YOU ARE TOO BIG TO CONTINUE");
        tutorial->_info_text[1] = renderer_create_text_sprite(tutorial->base._renderer, tutorial->base._info_font, "CHANGE TO SMALL SIZE WITH ( OR )");
        tutorial->phase = TUTORIAL_PHASE_CHANGE_TO_SMALL;
    }
}

static void update_phase_change_to_small(TutorialScreen* tutorial, float time_delta) {
    if (input_button_is_down(tutorial->base._input, CONTROLLER_1, CONTROLLER_BUTTON_C_LEFT) || input_button_is_down(tutorial->base._input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_LEFT) ) {
        clear_info_texts(tutorial);
        player_try_set_size(tutorial->base.player, PLAYER_SIZE_SMALL);
        tutorial->phase = TUTORIAL_PHASE_RUN_THROUGH_SMALL_AREA;
    }
}

static void update_phase_run_though_small_area(TutorialScreen* tutorial, float time_delta) {
    playing_screen_base_update(&tutorial->base, time_delta);

    if (tutorial->base.player->entity.position.x >= 88.0) {
        clear_info_texts(tutorial);
        tutorial->_info_text[0] = renderer_create_text_sprite(tutorial->base._renderer, tutorial->base._info_font, "RETURN TO NORMAL SIZE WITH * OR +");
        tutorial->phase = TUTORIAL_PHASE_CHANGE_TO_MEDIUM;
    }
}

static void update_phase_change_to_medium(TutorialScreen* tutorial, float time_delta) {
    if (input_button_is_down(tutorial->base._input, CONTROLLER_1, CONTROLLER_BUTTON_C_UP) || input_button_is_down(tutorial->base._input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_UP) ) {
        player_try_set_size(tutorial->base.player, PLAYER_SIZE_MEDIUM);
        clear_info_texts(tutorial);
        tutorial->phase = TUTORIAL_PHASE_CONTINUE_TO_LEDGE_JUMP;
    }
}

static void update_phase_continue_to_ledge_jump(TutorialScreen* tutorial, float time_delta) {
    playing_screen_base_update(&tutorial->base, time_delta);

    if (tutorial->base.player->prev_pos.x < 96.5f && tutorial->base.player->entity.position.x >= 96.5f) {
        clear_info_texts(tutorial);
        tutorial->_info_text[0] = renderer_create_text_sprite(tutorial->base._renderer, tutorial->base._info_font, "JUMP TO THE LEDGE WITH # OR $");
        tutorial->phase = TUTORIAL_PHASE_START_JUMP_TO_LEDGE;
    }
}

static void update_phase_start_jump_to_ledge(TutorialScreen* tutorial, float time_delta) {
    if (input_button_is_down(tutorial->base._input, CONTROLLER_1, CONTROLLER_BUTTON_A) || input_button_is_down(tutorial->base._input, CONTROLLER_1, CONTROLLER_BUTTON_Z) ) {
        player_try_jump(tutorial->base.player);
        clear_info_texts(tutorial);
        tutorial->phase = TUTORIAL_PHASE_JUMPING_TO_LEDGE;
    }
}

static void update_phase_jumping_to_ledge(TutorialScreen* tutorial, float time_delta) {
    playing_screen_base_update(&tutorial->base, time_delta);

    if (tutorial->base.player->prev_pos.x < 97.5f && tutorial->base.player->entity.position.x >= 97.5f) {
        tutorial->_info_text[0] = renderer_create_text_sprite(tutorial->base._renderer, tutorial->base._info_font, "YOU WILL NOT FIT ON THE LEDGE");
        tutorial->_info_text[1] = renderer_create_text_sprite(tutorial->base._renderer, tutorial->base._info_font, "CHANGE TO SMALL IN AIR WITH ( OR )");
        tutorial->phase = TUTORIAL_PHASE_MID_AIR_SIZE_CHANGE;
    }
}

static void update_phase_mid_air_size_change(TutorialScreen* tutorial, float time_delta) {
    if (input_button_is_down(tutorial->base._input, CONTROLLER_1, CONTROLLER_BUTTON_C_LEFT) || input_button_is_down(tutorial->base._input, CONTROLLER_1, CONTROLLER_BUTTON_DPAD_LEFT) ) {
        player_try_set_size(tutorial->base.player, PLAYER_SIZE_SMALL);
        clear_info_texts(tutorial);
        tutorial->phase = TUTORIAL_PHASE_RUN_TO_PRACTICE;
    }
}

static void update_phase_run_to_practice(TutorialScreen* tutorial, float time_delta) {
    playing_screen_base_update(&tutorial->base, time_delta);

    if (tutorial->base.player->prev_pos.x < 120.0f && tutorial->base.player->entity.position.x >= 120.0f) {
        player_try_set_size(tutorial->base.player, PLAYER_SIZE_MEDIUM);
    }
    else if (tutorial->base.player->prev_pos.x < 130.0f && tutorial->base.player->entity.position.x >= 130.0f) {
        clear_info_texts(tutorial);

        tutorial->base.level->start_pos.x = 130.0f;
        player_clear_stats(tutorial->base.player);
        tutorial->phase = TUTORIAL_PHASE_PRACTICE;
    }
}

static void update_phase_practice(TutorialScreen* tutorial, float time_delta) {
    playing_screen_base_update(&tutorial->base, time_delta);

    Player* player = tutorial->base.player;

    if (player->prev_pos.x < 130.5f && player->entity.position.x >= 130.5f) {
        clear_info_texts(tutorial);
        tutorial->_info_text[0] = renderer_create_text_sprite(tutorial->base._renderer, tutorial->base._info_font, "NAVIGATE ALL OBSTACLES TO FINISH");
        tutorial->_info_text[1] = renderer_create_text_sprite(tutorial->base._renderer, tutorial->base._info_font, "THE TUTORIAL");
        player->process_input = 1;
    }

    if (player->prev_pos.x < tutorial->base.level->goal_dist && player->entity.position.x >= tutorial->base.level->goal_dist) {
        clear_info_texts(tutorial);
    }

    if (tutorial->practice_hint_index >= PRACTICE_HINT_COUNT)
        return;

    if (player->prev_pos.x < practice_hints[tutorial->practice_hint_index].pos_x && player->entity.position.x >= practice_hints[tutorial->practice_hint_index].pos_x) {
        clear_info_texts(tutorial);
        tutorial->_info_text[0] = renderer_create_text_sprite(tutorial->base._renderer, tutorial->base._info_font, practice_hints[tutorial->practice_hint_index++].hint);
    }
}

void tutorial_screen_update(TutorialScreen* tutorial, float time_delta){
    switch(tutorial->phase) {
        case TUTORIAL_PHASE_WAITING_FOR_TELEPORT_IN:
            playing_screen_base_update(&tutorial->base, time_delta);
            break;
        case TUTORIAL_PHASE_INFO:
            update_phase_info(tutorial);
            break;

        case TUTORIAL_STATE_BASIC_MOVEMENT:
            update_phase_basic_movement(tutorial, time_delta);
            break;

        case TUTORIAL_STATE_JUMP:
            update_phase_jump(tutorial, time_delta);
            break;

        case TUTORIAL_PHASE_RUN_TO_LARGE_CHANGE:
            update_phase_run_to_jump2(tutorial, time_delta);
            break;

        case TUTORIAL_PHASE_CHANGE_TO_LARGE:
            update_phase_change_to_large(tutorial, time_delta);
            break;

        case TUTORIAL_PHASE_CLEAR_LARGE_OBSTACLE:
            update_phase_clear_large_obstacle(tutorial, time_delta);
            break;

        case TUTORIAL_PHASE_SMASH_BRICKS:
            update_phase_smash_bricks(tutorial, time_delta);
            break;

        case TUTORIAL_PHASE_CHANGE_TO_SMALL:
            update_phase_change_to_small(tutorial, time_delta);
            break;

        case TUTORIAL_PHASE_RUN_THROUGH_SMALL_AREA:
            update_phase_run_though_small_area(tutorial, time_delta);
            break;

        case TUTORIAL_PHASE_CHANGE_TO_MEDIUM:
            update_phase_change_to_medium(tutorial, time_delta);
            break;

        case TUTORIAL_PHASE_CONTINUE_TO_LEDGE_JUMP:
            update_phase_continue_to_ledge_jump(tutorial, time_delta);
            break;

        case TUTORIAL_PHASE_START_JUMP_TO_LEDGE:
            update_phase_start_jump_to_ledge(tutorial, time_delta);
            break;

        case TUTORIAL_PHASE_JUMPING_TO_LEDGE:
            update_phase_jumping_to_ledge(tutorial, time_delta);
            break;

        case TUTORIAL_PHASE_MID_AIR_SIZE_CHANGE:
            update_phase_mid_air_size_change(tutorial, time_delta);
            break;

        case TUTORIAL_PHASE_RUN_TO_PRACTICE:
            update_phase_run_to_practice(tutorial, time_delta);
            break;

        case TUTORIAL_PHASE_PRACTICE:
            update_phase_practice(tutorial, time_delta);
            break;
    }
}

void tutorial_screen_draw(TutorialScreen* tutorial) {
    playing_screen_base_draw(&tutorial->base);

    if (tutorial->base._pause_dialog.base.shown || tutorial->base._attempt_dialog.base.shown)
        return;

    int y_pos = 10;
    for (int i = 0; i < INFO_TEXT_LINE_COUNT; i++) {
        Sprite* info_text = tutorial->_info_text[i];
        if (!info_text) continue;

        renderer_draw_sprite(tutorial->base._renderer, info_text, 10, y_pos);
        y_pos += 20;
    }
}

void on_teleport_in_complete(void* user_data) {
    TutorialScreen* tutorial = (TutorialScreen*)user_data;

    if (tutorial->phase == TUTORIAL_PHASE_PRACTICE) {
        tutorial->practice_hint_index = 0;
        player_start(tutorial->base.player);
    }
    else if (tutorial->phase == TUTORIAL_PHASE_WAITING_FOR_TELEPORT_IN) {
        tutorial->phase = TUTORIAL_PHASE_INFO;
        clear_info_texts(tutorial);
        tutorial->_info_text[0] = renderer_create_text_sprite(tutorial->base._renderer, tutorial->base._info_font, "RETRO DASH TUTORIAL");
        tutorial->_info_text[1] = renderer_create_text_sprite(tutorial->base._renderer, tutorial->base._info_font, "TO BEGIN PRESS: !");
    }
}

void on_teleport_out_complete(void* user_data) {
    TutorialScreen* tutorial = (TutorialScreen*)user_data;

    attempt_dialog_show(&tutorial->base._attempt_dialog);
    audio_pause_music(tutorial->base._audio);
}

void on_dialog_return(void* user_data) {
    TutorialScreen* state = (TutorialScreen*)user_data;

    state->transition = GAME_STATE_TITLE;
}
