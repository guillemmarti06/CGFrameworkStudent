//
//  ParticleSystem.h
//  ComputerGraphics
//
//  Created by GUILLEM on 21/1/26.
//

#pragma once
#include "framework.h"
#include "image.h"

class ParticleSystem
{
public:
    static const int MAX_PARTICLES = 500; // prueba 300-1000

    struct Particle {
        Vector2 position;
        Vector2 velocity;   // dirección * velocidad
        Color color;
        float acceleration; // acelera un poquito
        float ttl;          // time to live
        bool inactive;
    };

    Particle particles[MAX_PARTICLES];

    int width = 0;
    int height = 0;

    void Init(int w, int h);
    void Render(Image* framebuffer);
    void Update(float dt);

private:
    float frand(float a, float b);
    Particle CreateParticle();
};

