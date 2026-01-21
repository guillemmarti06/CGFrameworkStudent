//
//  ParticleSystem.h
//  ComputerGraphics
//
//  Created by GUILLEM on 21/1/26.
//

#pragma once
#include "framework.h"
#include "image.h"

// We kept it simple; fixed-size array
class ParticleSystem
{
public:
    // We chose 500 because it looks good and still runs fast.
    static const int MAX_PARTICLES = 500;

    struct Particle {
        Vector2 position;
        Vector2 velocity;   // Direction + speed (pixels per second basically)
        Color color;
        float acceleration; // Small acceleration to make motion more dynamic
        float ttl;          // time to live
        bool inactive;
    };

    // fixed array
    Particle particles[MAX_PARTICLES];

    int width = 0;
    int height = 0;

    // create particles with random initial values
    // We pass w/h so the particle system knows the "screen limits"
    void Init(int w, int h);
    void Render(Image* framebuffer);  // Here we just draw pixels (1 pixel per particle)
    void Update(float dt);  // position += velocity * dt

private:
    float frand(float a, float b);
    Particle CreateParticle();  // Creates a new particle with random properties.
};

