#include "rect.h"

int rect_intersection(Rect* a, Rect* b) {
    if (a->x > b->x + b->w)
        return 0;

    if (a->x + a->w < b->x)
        return 0;
        

    if (a->y > b->y + b->h)
        return 0;


    if (a->y + a->h < b->y)
        return 0;

    return 1;
}

void rect_set(Rect* r, int x, int y, int w, int h) {
    r->x = x;
    r->y = y;
    r->w = w;
    r->h = h;
}
