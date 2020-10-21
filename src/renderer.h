#ifndef RENDERER_H
#define RENDERER_H

typedef struct Sprite Sprite;
typedef struct Renderer Renderer;

void renderer_draw_sprite(Renderer* renderer, Sprite* sprite, int x, int y, int frame);

#endif