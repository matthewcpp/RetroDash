#ifndef TUTORIAL
#define TUTORIAL

// icons:
// https://www.ssbwiki.com/Category:Button_icons_(N64)

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
    TUTORIAL_PHASE_PRACTICE,
} TutorialPhase;

typedef struct {
    StatePlayingBase base;
    GameState transition;
    TutorialPhase phase;
    Sprite* _info_text[INFO_TEXT_LINE_COUNT];
    int practice_hint_index;
} StateTutorial;

StateTutorial* state_tutorial_create(Audio* audio, Input* input, Renderer* renderer);
void state_tutorial_destroy(StateTutorial* tutorial);
void state_tutorial_update(StateTutorial* tutorial, float time_delta);
void state_tutorial_draw(StateTutorial* tutorial);

#endif