#include "camera.h"

#include <math.h>
#include <stdlib.h>

Camera* camera_create(int screen_width, int screen_height) {
    Camera* camera = calloc(1, sizeof(Camera));

    camera->screen_size.x = (float)screen_width;
    camera->screen_size.y = (float)screen_height;

    return camera;
}

void camera_set_target(Camera* camera, Vec2* target) {
    camera->target = target;
}

void camera_set_tile_size(Camera* camera, int width, int height) {
    camera->tile_size.x = (float)width;
    camera->tile_size.y = (float)height;
}

void camera_set_offset(Camera* camera, float x, float y) {
    camera->offset.x = x;
    camera->offset.y = y;
}

void camera_world_pos_to_screen_pos(Camera* camera, Vec2* world_pos, Point* screen_pos) {
    float origin_x = camera->target->x + camera->offset.x;
    float origin_y = camera->target->y + camera->offset.y;

    screen_pos->x = (int)((world_pos->x - origin_x) * camera->tile_size.x);
    float screen = ((origin_y - world_pos->y ) * camera->tile_size.y);
    screen_pos->y = (int)(screen);
}

void camera_screen_pos_to_world_pos(Camera* camera, Point* screen_pos, Vec2* world_pos) {
    float origin_x = camera->target->x + camera->offset.x;
    float origin_y = camera->target->y + camera->offset.y;

    world_pos->x = origin_x + (float)screen_pos->x / camera->tile_size.x;
    world_pos->y = origin_y - (float)screen_pos->y / camera->tile_size.y;
}