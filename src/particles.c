#include "particles.h"

#include <stdlib.h>
#include <string.h>

void particle_group_clear(ParticleGroup* particle_group) {
    particle_group->start = 0;
    particle_group->count = 0;
}

void particle_group_init(ParticleGroup* particle_group, int capacity){
    particle_group->particles = calloc(capacity, sizeof(Particle));
    particle_group->capacity = capacity;
    particle_group_clear(particle_group);
}

void particle_group_uninit(ParticleGroup* particle_group) {
    free(particle_group->particles);
}

void particle_group_add(ParticleGroup* particle_group, Particle* particle) {
    int write_index;
    if (particle_group->count < particle_group->capacity) {
        write_index = (particle_group->start + particle_group->count) % particle_group->capacity;
        particle_group->count += 1;
    }
    else {
        write_index = particle_group->start;
        particle_group->start = (particle_group->start + 1) % particle_group->capacity;
    }

    memcpy(particle_group->particles + write_index, particle, sizeof(Particle));
}

void particle_group_pop_start(ParticleGroup* particle_group) {
    if (particle_group->count > 0) {
        particle_group->start = (particle_group->start + 1) % particle_group->capacity;
        particle_group->count -= 1;
    }
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

// removes inactive particles from the front of the circular buffer.  Attempt to save on draw calls.
static void prune_inative_particles(BrickParticles* brick_particles) {
    while (brick_particles->_group.count > 0) {
        Particle* particle = brick_particles->_group.particles + brick_particles->_group.start;

        // particle has gone off screen
        if (particle->position.x + PARTICLE_SIZE < brick_particles->_camera->_origin.x ||
            particle->position.y - PARTICLE_SIZE < 0.0f) {
            particle_group_pop_start(&brick_particles->_group);
        }
        else {
            return;
        }
    }
}

void brick_particles_update(BrickParticles* brick_particles, float time_delta) {
    prune_inative_particles(brick_particles);

    // move all brick particles
    for (int i = 0; i < brick_particles->_group.count; i++) {
        int particle_index = (brick_particles->_group.start + i) % brick_particles->_group.capacity;
        Particle* particle = brick_particles->_group.particles + particle_index;

        particle->velocity.y -= GRAVITY * time_delta;
        particle->position.y += particle->velocity.y * time_delta;
        particle->position.x += particle->velocity.x * time_delta;
    }
}

void brick_particles_draw(BrickParticles* brick_particles) {
    int current = brick_particles->_group.start;
    float particle_screen_size = 24.0f * PARTICLE_SIZE;
    float sprite_scale_x = particle_screen_size / (float)sprite_horizontal_frame_size(brick_particles->_sprite);
    float sprite_scale_y = particle_screen_size / (float)sprite_vertical_frame_size(brick_particles->_sprite);

    for (int i = 0; i < brick_particles->_group.count; i++) {
        int particle_index = (brick_particles->_group.start + i) % brick_particles->_group.capacity;
        Particle* particle = brick_particles->_group.particles + particle_index;


        if (particle->position.x >= brick_particles->_camera->_origin.x && particle->position.y - PARTICLE_SIZE >= 0.0f) {
            Point screen_pos;
            camera_world_pos_to_screen_pos(brick_particles->_camera, &particle->position, &screen_pos);

            renderer_draw_scaled_sprite(brick_particles->_renderer, brick_particles->_sprite,
                        screen_pos.x, screen_pos.y, sprite_scale_x, sprite_scale_y, brick_particles->_frame);
        }

        current = (current + 1) % brick_particles->_group.count;
    }
}

void brick_particles_clear(BrickParticles* brick_particles) {
    particle_group_clear(&brick_particles->_group);
}
