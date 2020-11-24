#ifndef TELEPORT_H
#define TELEPORT_H

#include "camera.h"
#include "player.h"
#include "renderer.h"

#include "rect.h"

typedef void(*TeleportCallback)(void*);

typedef enum {
    TELEPORT_STATUS_INACTIVE,
    TELEPORT_STATUS_IN
} TeleportStatus;

typedef struct {
    Renderer* _renderer;
    TeleportStatus status;

    TeleportCallback _callback;
    void* _callback_data;

    float _time;

    Point _start_position;
    Point _target_position;
} Teleport;

void teleport_init(Teleport* teleport, Renderer* renderer);
void teleport_update(Teleport* teleport, float time_delta);
void teleport_draw(Teleport* teleport);

int teleport_is_active(Teleport* teleport);

/**
 * Performs a beam animation from the top of the screen to the Player's current position.
 * Calls the user supplied function on completion
 */
void teleport_in(Teleport* teleport, Player* player, Camera* camera, TeleportCallback callback, void* user_data);

#endif