#ifndef PARTICLES_H
#define PARTICLES_H

#include "rect.h"
#include "camera.h"
#include "renderer.h"

typedef struct {
    Vec2 position;
    Vec2 velocity;
} Particle;

typedef struct {
    int start;
    int end;
    int count;
    Particle* particles;
} ParticleGroup;

void particle_group_init(ParticleGroup* particle_group, int count);
void particle_group_uninit(ParticleGroup* particle_group);
void particle_group_add(ParticleGroup* particle_group, Particle* particle);
void particle_group_pop_start(ParticleGroup* particle_group);
void particle_group_clear(ParticleGroup* particle_group);

typedef struct {
    ParticleGroup _group;
    Renderer* _renderer;
    Camera* _camera;
    Sprite* _sprite;
    int _frame;
} BrickParticles;

void brick_particles_init(BrickParticles* brick_particles, Camera* camera, Renderer* renderer);
void brick_particles_uninit(BrickParticles* brick_particles);

void brick_particles_clear(BrickParticles* brick_particles);
void brick_particles_add(BrickParticles* brick_particles, Vec2* position);
void brick_particles_update(BrickParticles* brick_particles, float time_delta);
void brick_particles_draw(BrickParticles* brick_particles);

#endif