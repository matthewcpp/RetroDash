#ifndef RECT_H
#define RECT_H

typedef struct {
    int x, y;
    int w, h;
} Rect;

int rect_intersection(Rect* a, Rect* b);
void rect_set(Rect* r, int x, int y, int w, int h);

typedef struct {
    int x, y;
} Point;

#endif