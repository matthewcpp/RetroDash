#include "tile_collision.h"

#include "intersection.h"

int tile_collision_spike(Entity* entity, int pos_x, int pos_y) {
    // the entity's line segment will be its bottom bounding box edge
    Vec2 entity_seg_a, entity_seg_b;
    entity_seg_a.x = entity->position.x - entity->size.x / 2.0f;
    entity_seg_a.y = entity->position.y;

    entity_seg_b.x = entity->position.x + entity->size.x / 2.0f;
    entity_seg_b.y = entity->position.y;

    // for the spike we will test two of the triangle sides
    Vec2 spike_seg_a, spike_seg_b;

    spike_seg_a.x = (float)pos_x;
    spike_seg_a.y = (float)pos_y;

    spike_seg_b.x = spike_seg_a.x + 0.5f;
    spike_seg_b.y = spike_seg_a.y + 1.0f;

    if (intersection_line_segments(&entity_seg_a, &entity_seg_b, &spike_seg_a, &spike_seg_b))
        return 1;

    spike_seg_a.x += 1.0f;

    return intersection_line_segments(&entity_seg_a, &entity_seg_b, &spike_seg_a, &spike_seg_b);
}