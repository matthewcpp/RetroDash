#include "particles.h"

#include <stdlib.h>
#include <string.h>

void particle_group_init(ParticleGroup* particle_group, int count){
    particle_group->particles = calloc(count, sizeof(Particle));
    particle_group->count = count;
    particle_group->start = 0;
    particle_group->end = 0;
}

void particle_group_uninit(ParticleGroup* particle_group) {
    free(particle_group->particles);
}

void particle_group_add(ParticleGroup* particle_group, Particle* particle) {
    particle_group->end = (particle_group->end + 1) % particle_group->count;

    if (particle_group->end == particle_group->start)
        particle_group->start = (particle_group->start + 1) % particle_group->count;

    memcpy(particle_group->particles + particle_group->end, particle, sizeof(Particle));
}

// note: assumes not empty!
void particle_group_pop_start(ParticleGroup* particle_group) {
    particle_group->start = (particle_group->start + 1) % particle_group->count;
}

void particle_group_clear(ParticleGroup* particle_group) {
    particle_group->start = particle_group->end;
}

// TODO: this should probably not be hard coded.
#define GRAVITY 36.0f
#define PARTICLE_SIZE 0.4f

Vec2 brick_velocities[2] = { {15.0f, 9.0f}, {7.0f, 15.0f}};
int velocity_index = 0;

void brick_particles_init(BrickParticles* brick_particles, Camera* camera, Renderer* renderer) {
    brick_particles->_renderer = renderer;
    brick_particles->_camera = camera;

    particle_group_init(&brick_particles->_group, 20);
}

void brick_particles_uninit(BrickParticles* brick_particles) {
    particle_group_uninit(&brick_particles->_group);
}

void brick_particles_add(BrickParticles* brick_particles, Vec2* position) {
    Particle particle;
    particle.position = *position;
    particle.velocity = brick_velocities[velocity_index];

    velocity_index = (velocity_index + 1) % 2;

    particle_group_add(&brick_particles->_group, &particle);
}

void brick_particles_update(BrickParticles* brick_particles, float time_delta) {
    int current = brick_particles->_group.start;

    // prune inactive particles at start of buffer
    while(current != brick_particles->_group.end) {
        Particle* particle = brick_particles->_group.particles + current;

        // particle has gone off screen
        if (particle->position.x + PARTICLE_SIZE < brick_particles->_camera->_origin.x) {
            particle_group_pop_start(&brick_particles->_group);
            current = brick_particles->_group.start;
        }
        else {
            break;
        }
    }

    current = brick_particles->_group.start;

    // move all brick particles
    while (current != brick_particles->_group.end) {
        Particle* particle = brick_particles->_group.particles + current;

        particle->velocity.y -= GRAVITY * time_delta;
        particle->position.y += particle->velocity.y * time_delta;
        particle->position.x += particle->velocity.x * time_delta;

        current = (current + 1) % brick_particles->_group.count;
    }
}

void brick_particles_draw(BrickParticles* brick_particles) {
    int current = brick_particles->_group.start;
    float particle_screen_size = 24.0f * PARTICLE_SIZE;
    float sprite_scale_x = particle_screen_size / (float)sprite_horizontal_frame_size(brick_particles->_sprite);
    float sprite_scale_y = particle_screen_size / (float)sprite_vertical_frame_size(brick_particles->_sprite);

    while(current != brick_particles->_group.end) {
        Point screen_pos;
        camera_world_pos_to_screen_pos(brick_particles->_camera, &brick_particles->_group.particles[current].position, &screen_pos);

        renderer_draw_scaled_sprite(brick_particles->_renderer, brick_particles->_sprite,
                                    screen_pos.x, screen_pos.y, sprite_scale_x, sprite_scale_y, brick_particles->_frame);

        current = (current + 1) % brick_particles->_group.count;
    }
}

void brick_particles_clear(BrickParticles* brick_particles) {
    particle_group_clear(&brick_particles->_group);
}
