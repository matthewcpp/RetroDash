#ifndef CAMERA_H
#define CAMERA_H

#include "rect.h"

typedef struct {
    Vec2* target;
    Vec2 offset;
    Vec2 screen_size;
    Vec2 tile_size;
} Camera;

Camera* camera_create(int screen_width, int screen_height);
void camera_set_target(Camera* camera, Vec2* target);
void camera_set_tile_size(Camera* camera, int width, int height);
void camera_set_offset(Camera* camera, float x, float y);

void camera_world_pos_to_screen_pos(Camera* camera, Vec2* world_pos, Point* screen_pos);
void camera_screen_pos_to_world_pos(Camera* camera, Point* screen_pos, Vec2* world_pos);

#endif