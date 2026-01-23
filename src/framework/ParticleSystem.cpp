//
//  ParticleSystem.cpp
//  ComputerGraphics
//
//  Created by GUILLEM on 21/1/26.
//
#include "ParticleSystem.h"

// just a random float helper (we use it everywhere for positions/speeds/colors)
float ParticleSystem::frand(float a, float b)
{
    return a + (b - a) * (float(rand()) / float(RAND_MAX));
}

ParticleSystem::Particle ParticleSystem::CreateParticle()
{
    Particle p;
    p.inactive = false;

    // We decided to spawn particles around the center to get a kind of starfield explosion effect (as in the example given)
    Vector2 center(width * 0.5f, height * 0.5f);

    // Start near the center
    p.position = center + Vector2(frand(-5.f, 5.f), frand(-5.f, 5.f));

    // Random direction (vx, vy) then normalize it so direction length = 1
    float vx = frand(-1.f, 1.f);
    float vy = frand(-1.f, 1.f);
    float len = std::sqrt(vx*vx + vy*vy);
    if (len < 0.0001f) len = 1.f;
    vx /= len; vy /= len;

    // Random speed
    float speed = frand(60.f, 250.f);  // px/s
    p.velocity = Vector2(vx, vy) * speed;

    // Small acceleration to make the motion more "alive"
    p.acceleration = frand(10.f, 80.f); // accelerates a bit
    p.ttl = frand(1.0f, 2.5f);

    // Random brighty color to look like stars
    int r = (int)frand(150, 255);
    int g = (int)frand(150, 255);
    int b = (int)frand(150, 255);
    p.color = Color(r, g, b);

    return p;
}

void ParticleSystem::Init(int w, int h)
{
    width = w;
    height = h;

    // Initialize particles
    for (int i = 0; i < MAX_PARTICLES; ++i)
        particles[i] = CreateParticle();
}

void ParticleSystem::Update(float dt)
{
    // Update each particle (basic):
    // position = position + velocity * dt
    // velocity = velocity + direction * acceleration * dt
    for (int i = 0; i < MAX_PARTICLES; ++i)
    {
        Particle& p = particles[i];
        if (p.inactive) continue;

        // p = p + v * dt
        p.position = p.position + p.velocity * dt;

        // Accelerate in the direction of movement (so it "speeds up" as it goes away)
        float speed = std::sqrt(p.velocity.x*p.velocity.x + p.velocity.y*p.velocity.y);
        if (speed > 0.0001f)
        {
            Vector2 dir = p.velocity * (1.0f / speed);
            p.velocity = p.velocity + dir * (p.acceleration * dt);
        }

        p.ttl -= dt;

        // If particle expires or leaves the screen, we just recreate it.
        if (p.ttl <= 0.f ||
            p.position.x < 0 || p.position.x >= width ||
            p.position.y < 0 || p.position.y >= height)
        {
            p = CreateParticle();
        }
    }
}

void ParticleSystem::Render(Image* framebuffer)
{
    // We draw particles as 1 pixel each
    for (int i = 0; i < MAX_PARTICLES; ++i)
    {
        Particle& p = particles[i];
        if (p.inactive) continue;

        int x = (int)p.position.x;
        int y = (int)p.position.y;
        
        // Always check bounds so we never write outside the framebuffer memory
        if (x >= 0 && x < framebuffer->width && y >= 0 && y < framebuffer->height)
            framebuffer->SetPixel(x, y, p.color);
    }
}
