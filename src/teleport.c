#include "teleport.h"

#include <stddef.h>

#define TELEPORT_TIME 0.5f
#define TELEPORT_WIDTH 8
#define TELEPORT_HEIGHT 24

static void teleport_reset(Teleport* teleport) {
    teleport->status = TELEPORT_STATUS_INACTIVE;
    teleport->_time = 0.0f;
    teleport->_callback = NULL;
    teleport->_callback_data = NULL;
}

void teleport_init(Teleport* teleport, Renderer* renderer) {
    teleport->_renderer = renderer;
    teleport_reset(teleport);
}

static void teleport_finish(Teleport* teleport) {
    if (teleport->_callback)
        teleport->_callback(teleport->_callback_data);

    teleport_reset(teleport);
}

void teleport_update(Teleport* teleport, float time_delta) {
    teleport->_time += time_delta;

    if (teleport->_time >= TELEPORT_TIME)
        teleport_finish(teleport);
}

void teleport_draw(Teleport* teleport) {
    float progress = teleport->_time / TELEPORT_TIME;
    int current_y = teleport->_start_position.y + (int)((teleport->_target_position.y - teleport->_start_position.y) * progress);

    Rect r;
    r.x = teleport->_target_position.x - TELEPORT_WIDTH / 2;
    r.y = current_y - TELEPORT_HEIGHT;
    r.w = TELEPORT_WIDTH;
    r.h = TELEPORT_HEIGHT;

    renderer_set_color(teleport->_renderer, 255, 239, 196, 255);
    renderer_draw_filled_rect(teleport->_renderer, &r);
}

void teleport_in(Teleport* teleport, Player* player, Camera* camera, TeleportCallback callback, void* user_data) {
    teleport->status = TELEPORT_STATUS_ACTIVE;

    camera_world_pos_to_screen_pos(camera, &player->entity.position, &teleport->_target_position);
    point_set(&teleport->_start_position, teleport->_target_position.x, 0);

    teleport->_callback = callback;
    teleport->_callback_data = user_data;
}

void teleport_out(Teleport* teleport, Player* player, Camera* camera, TeleportCallback callback, void* user_data) {
    teleport->status = TELEPORT_STATUS_ACTIVE;

    camera_world_pos_to_screen_pos(camera, &player->entity.position, &teleport->_start_position);
    point_set(&teleport->_target_position, teleport->_start_position.x, -TELEPORT_HEIGHT);

    teleport->_callback = callback;
    teleport->_callback_data = user_data;
}
