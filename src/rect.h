#ifndef RECT_H
#define RECT_H

typedef struct {
    float x, y;
    float w, h;
} Rect;

int rect_intersection(Rect* a, Rect* b);
void rect_set(Rect* r, float x, float y, float w, float h);

#endif