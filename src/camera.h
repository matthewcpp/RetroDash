#ifndef CAMERA_H
#define CAMERA_H

#include "rect.h"

typedef struct {
    Box* _target;
    Vec2 _offset;
    Vec2 _screen_size;
    Vec2 _tile_size;
    Vec2 _origin;
} Camera;

Camera* camera_create(int screen_width, int screen_height);
void camera_destroy(Camera* camera);

/**
 * Updates camera origin based on the target.  This should be called after the target (the player) has been updated.
 */
void camera_update(Camera* camera);

/**
 * Sets the target to track.  This should be set to the player's bounding box.
 */
void camera_set_target(Camera* camera, Box* target);

/**
 * Set this to the tile size of the level.  This is used for internal calculations.
 */
void camera_set_tile_size(Camera* camera, int width, int height);
/**
 * Sets camera safe margins.  The Camera will ensure that the player sprite reamins at least this far away from the frame during gameplay.
 */
void camera_set_safe_margins(Camera* camera, float x, float y);

/**
 * Converts a world space point to a screen position.
 * The origin of screen space has the origin at the top right corner.
 */
void camera_world_pos_to_screen_pos(Camera* camera, Vec2* world_pos, Point* screen_pos);

/**
 * Converts a 2D point from screen space into world space.
 */
void camera_screen_pos_to_world_pos(Camera* camera, Point* screen_pos, Vec2* world_pos);

#endif