#include "intersection.h"

// https://en.wikipedia.org/wiki/Line–line_intersection
int intersection_line_segments(Vec2* p1, Vec2* p2, Vec2* p3, Vec2* p4) {
    float x12 = p1->x - p2->x;
    float x13 = p1->x - p3->x;
    float x34 = p3->x - p4->x;

    float y12 = p1->y - p2->y;
    float y34 = p3->y - p4->y;
    float y13 = p1->y - p3->y;

    float td = x12 * y34 - y12 * x34;
    if (td == 0.0f)
        return 0;

    float tn = x13 * y34 - y13 * x34;
    float t = tn / td;

    return t >= 0.0f && t <= 1.0f;
}