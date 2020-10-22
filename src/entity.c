#include "entity.h"

#include <stdlib.h>

Entity* entity_create(Renderer* renderer) {
	Entity* entity = malloc(sizeof(Entity));

	entity->_renderer = renderer;

	return entity;
}

void entity_draw(Entity* entity) {
	renderer_set_color(entity->_renderer, 0, 255, 0, 255);
	renderer_draw_filled_rect(entity->_renderer, &entity->rect);
}