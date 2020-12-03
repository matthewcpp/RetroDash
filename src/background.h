#ifndef BACKGROUND
#define BACKGROUND

#include "renderer.h"

typedef struct {
    Renderer* _renderer;
    float _offset;
} Background;

void background_init(Background* background, Renderer* renderer);
void background_update(Background* background, float time_delta);
void background_draw(Background* background);
void background_reset(Background* background);

#endif