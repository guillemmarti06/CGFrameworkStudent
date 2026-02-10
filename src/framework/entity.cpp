//
//  entity.cpp
//  ComputerGraphics
//
//  Created by GUILLEM on 10/2/26.
//

#include "entity.hpp"

Entity::Entity()
{
    model.SetIdentity();
}

void Entity::Update(float dt)
{
    if (!animated)
        return;

    // Rotate around Y axis (simple animation for the assignment)
    Matrix44 R;
    R.MakeRotationMatrix(dt * rotation_speed, Vector3(0.0f, 1.0f, 0.0f));
    model = R * model;
}

bool Entity::IsInsideClipSpace(const Vector3& ndc) const
{
    // After projection (and perspective divide), valid visible space is the cube [-1, 1]
    return (ndc.x >= -1.0f && ndc.x <= 1.0f &&
            ndc.y >= -1.0f && ndc.y <= 1.0f &&
            ndc.z >= -1.0f && ndc.z <= 1.0f);
}

Vector2 Entity::NDCToScreen(const Vector3& ndc, int width, int height) const
{
    // NDC coordinates go from -1..1, screen goes from 0..(size-1)
    Vector2 s;
    s.x = (ndc.x * 0.5f + 0.5f) * (width - 1);
    s.y = (ndc.y * 0.5f + 0.5f) * (height - 1);
    return s;
}

void Entity::RenderWireframe(Image& framebuffer, Camera& camera, const Color& color) const
{
    const std::vector<Vector3>& vertices = mesh.GetVertices();
    if (vertices.empty())
        return;

    const int w = framebuffer.width;
    const int h = framebuffer.height;

    // Each 3 vertices define one triangle (Mesh stores triangles)
    for (size_t i = 0; i + 2 < vertices.size(); i += 3)
    {
        // Local -> World (model matrix)
        Vector3 w0 = model * vertices[i];
        Vector3 w1 = model * vertices[i + 1];
        Vector3 w2 = model * vertices[i + 2];

        // World -> NDC (camera projection)
        Vector3 p0 = camera.ProjectVector(w0);
        Vector3 p1 = camera.ProjectVector(w1);
        Vector3 p2 = camera.ProjectVector(w2);

        // Simple clip test: skip triangles fully outside (good enough for this lab)
        // (Assignment mentions checking if projected vertex is inside [-1..1] cube.)
        if (!IsInsideClipSpace(p0) && !IsInsideClipSpace(p1) && !IsInsideClipSpace(p2))
            continue;

        Vector2 s0 = NDCToScreen(p0, w, h);
        Vector2 s1 = NDCToScreen(p1, w, h);
        Vector2 s2 = NDCToScreen(p2, w, h);

        // Draw triangle edges (wireframe) using DDA
        framebuffer.DrawLineDDA((int)s0.x, (int)s0.y, (int)s1.x, (int)s1.y, color);
        framebuffer.DrawLineDDA((int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, color);
        framebuffer.DrawLineDDA((int)s2.x, (int)s2.y, (int)s0.x, (int)s0.y, color);
    }
}

