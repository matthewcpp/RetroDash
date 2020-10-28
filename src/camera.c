#include "camera.h"

#include <math.h>
#include <stdlib.h>

Camera* camera_create(int screen_width, int screen_height) {
    Camera* camera = calloc(1, sizeof(Camera));

    camera->_screen_size.x = (float)screen_width;
    camera->_screen_size.y = (float)screen_height;

    return camera;
}

void camera_update(Camera* camera) {
    camera->_origin.x = camera->_target->x + camera->_offset.x;
    camera->_origin.y = camera->_target->y + + camera->_target->h + camera->_offset.y;
    if (camera->_origin.y < 10.0f)
        camera->_origin.y = 10.0f;
}

void camera_set_target(Camera* camera, Box* target) {
    camera->_target = target;
}

void camera_set_tile_size(Camera* camera, int width, int height) {
    camera->_tile_size.x = (float)width;
    camera->_tile_size.y = (float)height;
}

void camera_set_safe_margins(Camera* camera, float x, float y) {
    camera->_offset.x = x;
    camera->_offset.y = y;
}

void camera_world_pos_to_screen_pos(Camera* camera, Vec2* world_pos, Point* screen_pos) {
    screen_pos->x = (int)((world_pos->x - camera->_origin.x) * camera->_tile_size.x);
    screen_pos->y = (int)((camera->_origin.y - world_pos->y ) * camera->_tile_size.y);
}

void camera_screen_pos_to_world_pos(Camera* camera, Point* screen_pos, Vec2* world_pos) {
    world_pos->x = camera->_origin.x + (float)screen_pos->x / camera->_tile_size.x;
    world_pos->y = camera->_origin.y - (float)screen_pos->y / camera->_tile_size.y;
}