#ifndef TUTORIAL
#define TUTORIAL

#include "playing_base.h"
#include "states.h"

#define INFO_TEXT_LINE_COUNT 2

typedef enum {
    TUTORIAL_PHASE_WAITING_FOR_TELEPORT_IN,
    TUTORIAL_PHASE_INFO,
    TUTORIAL_STATE_BASIC_MOVEMENT,
    TUTORIAL_STATE_JUMP,
    TUTORIAL_PHASE_RUN_TO_LARGE_CHANGE,
    TUTORIAL_PHASE_CHANGE_TO_LARGE,
    TUTORIAL_PHASE_CLEAR_LARGE_OBSTACLE,
    TUTORIAL_PHASE_SMASH_BRICKS,
    TUTORIAL_PHASE_CHANGE_TO_SMALL,
    TUTORIAL_PHASE_RUN_THROUGH_SMALL_AREA,
    TUTORIAL_PHASE_CHANGE_TO_MEDIUM,
    TUTORIAL_PHASE_CONTINUE_TO_LEDGE_JUMP,
    TUTORIAL_PHASE_START_JUMP_TO_LEDGE,
    TUTORIAL_PHASE_JUMPING_TO_LEDGE,
    TUTORIAL_PHASE_MID_AIR_SIZE_CHANGE,
    TUTORIAL_PHASE_RUN_TO_PRACTICE,
    TUTORIAL_PHASE_PRACTICE,
} TutorialPhase;

typedef struct {
    PlayingScreenBase base;
    GameState transition;
    TutorialPhase phase;
    Sprite* _info_text[INFO_TEXT_LINE_COUNT];
    int practice_hint_index;
} TutorialScreen;

TutorialScreen* tutorial_screen_create(Audio* audio, Input* input, Renderer* renderer);
void tutorial_screen_destroy(TutorialScreen* tutorial);
void tutorial_screen_update(TutorialScreen* tutorial, float time_delta);
void tutorial_screen_draw(TutorialScreen* tutorial);

#endif