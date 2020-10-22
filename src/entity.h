#ifndef ENTITY_H
#define ENTITY_C

#include "rect.h"
#include "renderer.h"

typedef struct  {
    Rect rect;
    Renderer* _renderer;
} Entity;

Entity* entity_create(Renderer* renderer);
void entity_draw(Entity* entity);

#endif