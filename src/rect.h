#ifndef RECT_H
#define RECT_H

typedef struct {
    int x, y;
    int w, h;
} Rect;

typedef struct {
    float x,y;
    float w, h;
} Box;

int rect_intersection(Rect* a, Rect* b);
void rect_set(Rect* r, int x, int y, int w, int h);

typedef struct {
    int x, y;
} Point;

typedef struct {
    float x, y;
} Vec2;

void point_set_from_vec2(Point* p, const Vec2* v);

#endif