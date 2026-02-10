//
//  entity.cpp
//  ComputerGraphics
//
//  Created by GUILLEM on 10/2/26.
//
#include "entity.hpp"
#include <cmath>

Entity::Entity()
{
    model.SetIdentity();
}

void Entity::Update(float seconds_elapsed)
{
    if (!animated)
        return;

    time_accum += seconds_elapsed;

    Matrix44 R;
    R.MakeRotationMatrix(time_accum * rotation_speed, Vector3(0.0f, 1.0f, 0.0f));

    Matrix44 T;
    T.MakeTranslationMatrix(0.0f, sinf(time_accum) * 0.25f, 0.0f);

    model = T * R;
}

bool Entity::InsideClipCube(const Vector3& p)
{
    return (p.x >= -1.0f && p.x <= 1.0f &&
            p.y >= -1.0f && p.y <= 1.0f &&
            p.z >= -1.0f && p.z <= 1.0f);
}

Vector2 Entity::ClipToScreen(const Vector3& clip_pos, int w, int h)
{
    Vector2 s;
    s.x = (clip_pos.x * 0.5f + 0.5f) * float(w - 1);
    s.y = (clip_pos.y * 0.5f + 0.5f) * float(h - 1);
    return s;
}

void Entity::RenderWireframe(Image& framebuffer, Camera& camera, const Color& color) const
{
    // IMPORTANT: requires const overload of Mesh::GetVertices() in mesh.h
    const std::vector<Vector3>& verts = mesh.GetVertices();
    if (verts.size() < 3)
        return;

    const int W = framebuffer.width;
    const int H = framebuffer.height;

    for (size_t i = 0; i + 2 < verts.size(); i += 3)
    {
        Vector3 v0_world = model * verts[i + 0];
        Vector3 v1_world = model * verts[i + 1];
        Vector3 v2_world = model * verts[i + 2];

        Vector3 p0 = camera.ProjectVector(v0_world);
        Vector3 p1 = camera.ProjectVector(v1_world);
        Vector3 p2 = camera.ProjectVector(v2_world);

        // Clip-test in [-1,1]^3
        if (!InsideClipCube(p0) || !InsideClipCube(p1) || !InsideClipCube(p2))
            continue;

        Vector2 s0 = ClipToScreen(p0, W, H);
        Vector2 s1 = ClipToScreen(p1, W, H);
        Vector2 s2 = ClipToScreen(p2, W, H);

        framebuffer.DrawLineDDA((int)s0.x, (int)s0.y, (int)s1.x, (int)s1.y, color);
        framebuffer.DrawLineDDA((int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, color);
        framebuffer.DrawLineDDA((int)s2.x, (int)s2.y, (int)s0.x, (int)s0.y, color);
    }
}
