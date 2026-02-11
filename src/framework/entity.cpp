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

void Entity::Render(Image* framebuffer, Camera* camera, FloatImage* zBuffer)
{
    if (!mesh || !camera || !framebuffer)
        return;

    // If Z is disabled, we just ignore the zbuffer pointer
    FloatImage* zb = useZBuffer ? zBuffer : NULL;

    auto clipToScreen = [framebuffer](const Vector3& p) -> Vector2
    {
        float x = (p.x * 0.5f + 0.5f) * (float)framebuffer->width;
        float y = (p.y * 0.5f + 0.5f) * (float)framebuffer->height;
        return Vector2(x, y);
    };

    auto isInsideClipCube = [](const Vector3& p) -> bool
    {
        return (p.x >= -1.0f && p.x <= 1.0f &&
                p.y >= -1.0f && p.y <= 1.0f &&
                p.z >= -1.0f && p.z <= 1.0f);
    };

    const std::vector<Vector3>& vertices = mesh->GetVertices();
    const std::vector<Vector2>& uvs = mesh->GetUVs();

    // We can still render without UVs if we are not using texture,
    // but if we want texture we need uvs.
    bool meshHasUVs = (uvs.size() == vertices.size());

    for (size_t i = 0; i + 2 < vertices.size(); i += 3)
    {
        Vector3 v0 = vertices[i];
        Vector3 v1 = vertices[i + 1];
        Vector3 v2 = vertices[i + 2];

        Vector3 w0 = model * v0;
        Vector3 w1 = model * v1;
        Vector3 w2 = model * v2;

        Vector3 p0 = camera->ProjectVector(w0);
        Vector3 p1 = camera->ProjectVector(w1);
        Vector3 p2 = camera->ProjectVector(w2);

        if (!isInsideClipCube(p0) || !isInsideClipCube(p1) || !isInsideClipCube(p2))
            continue;

        Vector2 s0 = clipToScreen(p0);
        Vector2 s1 = clipToScreen(p1);
        Vector2 s2 = clipToScreen(p2);

        // Wireframe mode
        if (mode == eRenderMode::WIREFRAME)
        {
            framebuffer->DrawLineDDA((int)s0.x, (int)s0.y, (int)s1.x, (int)s1.y, Color::WHITE);
            framebuffer->DrawLineDDA((int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, Color::WHITE);
            framebuffer->DrawLineDDA((int)s2.x, (int)s2.y, (int)s0.x, (int)s0.y, Color::WHITE);
            continue;
        }

        // Filled modes need (x,y,z)
        Vector3 sp0(s0.x, s0.y, p0.z);
        Vector3 sp1(s1.x, s1.y, p1.z);
        Vector3 sp2(s2.x, s2.y, p2.z);

        // Build triangle info
        sTriangleInfo tri;
        tri.p0 = sp0; tri.p1 = sp1; tri.p2 = sp2;

        // Default UVs
        tri.uv0 = Vector2(0,0);
        tri.uv1 = Vector2(0,0);
        tri.uv2 = Vector2(0,0);

        if (meshHasUVs)
        {
            tri.uv0 = uvs[i];
            tri.uv1 = uvs[i + 1];
            tri.uv2 = uvs[i + 2];
        }

        // Set debug vertex colors
        tri.c0 = Color::RED;
        tri.c1 = Color::GREEN;
        tri.c2 = Color::BLUE;

        tri.texture = texture;

        // Triangles: plain color
        // Triangles Interpolated: texture (UV interp) OR vertex color per vertex (barycentric)
        if (mode == eRenderMode::TRIANGLES)
        {
            // plain color (all same -> solid)
            Color plain(180, 180, 180);
            tri.c0 = plain;
            tri.c1 = plain;
            tri.c2 = plain;

            tri.useTexture = false;
        }
        else // Triangles_interpolated
        {
            // If T says texture, try texture. If no texture or no uvs -> fallback to vertex colors
            bool canUseTexture = (useTexture && tri.texture != NULL && meshHasUVs);
            tri.useTexture = canUseTexture ? true : false;
        }

        framebuffer->DrawTriangleInterpolated(tri, zb);
    }
}


void Entity::Update(float seconds_elapsed)
{
    // A very simple time accumulator
    static float global_time = 0.0f;
    global_time += seconds_elapsed;

    float t = global_time * speed;

    // Build model matrix using TRS (as said on the slides)
    Matrix44 T, R, S;

    float y = sinf(t) * 0.2f;                 // translate up/down
    float angle = t;                          // rotate over time
    float scale = base_scale * (1.0f + 0.2f * sinf(t));  // scale oscillation

    T.MakeTranslationMatrix(base_position.x, base_position.y + y, base_position.z);
    R.MakeRotationMatrix(angle, Vector3(0,1,0));
    S.MakeScaleMatrix(scale, scale, scale);

    model = T * R * S;
}
