#ifndef TUTORIAL
#define TUTORIAL

// icons:
// https://www.ssbwiki.com/Category:Button_icons_(N64)

#include "playing_base.h"
#include "states.h"

typedef enum {
TUTORIAL_PHASE_TELEPORT_IN
} TutorialPhase;

typedef struct {
    StatePlayingBase base;
    GameState transition;
} StateTutorial;

StateTutorial* state_tutorial_create(Audio* audio, Input* input, Renderer* renderer);
void state_tutorial_destroy(StateTutorial* tutorial);
void state_tutorial_update(StateTutorial* tutorial, float time_delta);
void state_tutorial_draw(StateTutorial* tutorial);

#endif