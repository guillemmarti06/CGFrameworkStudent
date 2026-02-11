//
//  entity.cpp
//  ComputerGraphics
//
//  Created by GUILLEM on 10/2/26.
//

#include "entity.h"
#include "mesh.h"

// Constructor: initialize pointers and set identity matrix
Entity::Entity()
{
    mesh = nullptr;
    model.SetIdentity();
    base_position = Vector3(0,0,0);
    base_scale = 1.0f;
    speed = 1.0f;
}

// Destructor: we do NOT delete mesh here on purpose.
Entity::~Entity()
{
}

void Entity::Render(Image* framebuffer, Camera* camera, const Color& c)
{
    if (!framebuffer || !camera || !mesh)
        return;

    // Mesh vertices are stored as triangles: every 3 vertices = 1 triangle
    const std::vector<Vector3>& v = mesh->GetVertices();
    if (v.empty())
        return;

    // Helper: check if a projected vertex is inside the clip cube [-1,1]^3
    auto isInsideClipCube = [](const Vector3& p) -> bool {
        return (p.x >= -1.0f && p.x <= 1.0f &&
                p.y >= -1.0f && p.y <= 1.0f &&
                p.z >= -1.0f && p.z <= 1.0f);
    };

    // convert clip space [-1,1] to screen pixels [0..width-1, 0..height-1]
    auto clipToScreen = [framebuffer](const Vector3& p) -> Vector2 {
        float x = (p.x * 0.5f + 0.5f) * (float)(framebuffer->width  - 1);
        float y = (p.y * 0.5f + 0.5f) * (float)(framebuffer->height - 1);
        return Vector2(x, y);
    };

    // Iterate triangles
    for (size_t i = 0; i + 2 < v.size(); i += 3)
    {
        // 1) Local -> World using model matrix
        Vector3 w0 = model * v[i + 0];
        Vector3 w1 = model * v[i + 1];
        Vector3 w2 = model * v[i + 2];

        // 2) World -> Clip (NDC) using camera viewprojection matrix
        // (Camera::ProjectVector already divides by w for perspective)
        Vector3 p0 = camera->ProjectVector(w0);
        Vector3 p1 = camera->ProjectVector(w1);
        Vector3 p2 = camera->ProjectVector(w2);

        // 3) Reject triangles outside the clip cube [-1,1]^3 (simple approach, no clipping)
        if (!isInsideClipCube(p0) || !isInsideClipCube(p1) || !isInsideClipCube(p2))
            continue;

        // 4) Clip -> Screen
        Vector2 s0 = clipToScreen(p0);
        Vector2 s1 = clipToScreen(p1);
        Vector2 s2 = clipToScreen(p2);

        // 5) Draw wireframe triangle with DDA lines (from Lab 1)
        framebuffer->DrawLineDDA((int)s0.x, (int)s0.y, (int)s1.x, (int)s1.y, c);
        framebuffer->DrawLineDDA((int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, c);
        framebuffer->DrawLineDDA((int)s2.x, (int)s2.y, (int)s0.x, (int)s0.y, c);
    }
}

void Entity::Update(float seconds_elapsed)
{
    // A very simple time accumulator
    static float global_time = 0.0f;
    global_time += seconds_elapsed;

    float t = global_time * speed;

    // Build model matrix using TRS
    Matrix44 T, R, S;

    float y = sinf(t) * 0.2f;                 // translate up/down
    float angle = t;                          // rotate over time
    float scale = base_scale * (1.0f + 0.2f * sinf(t));  // scale oscillation

    T.MakeTranslationMatrix(base_position.x, base_position.y + y, base_position.z);
    R.MakeRotationMatrix(angle, Vector3(0,1,0));
    S.MakeScaleMatrix(scale, scale, scale);

    model = T * R * S;
}
