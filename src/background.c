#include "background.h"

#define BACKGROUND_GRID_SIZE 32
#define BACKGROUND_SPEED 40.0f

void background_init(Background* background, Renderer* renderer) {
    background->_renderer = renderer;
    background_reset(background);
}

void background_update(Background* background, float time_delta) {
    background->_offset -= time_delta * BACKGROUND_SPEED;

    if (background->_offset < 0.0f)
        background->_offset += BACKGROUND_GRID_SIZE;
}

void background_draw(Background* background) {
    renderer_set_color(background->_renderer, 33, 7, 58, 255);
    renderer_draw_grid(background->_renderer, (int)background->_offset, BACKGROUND_GRID_SIZE);
}

void background_reset(Background* background) {
    background->_offset = (float)BACKGROUND_GRID_SIZE;
}