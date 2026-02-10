//
//  entity.hpp
//  ComputerGraphics
//
//  Created by GUILLEM on 10/2/26.
//

#pragma once

#include "framework.h"
#include "mesh.h"
#include "camera.h"
#include "image.h"

class Entity
{
public:
    Mesh mesh;
    Matrix44 model;

    // Simple animation flags (used in task 2.4)
    bool animated = false;
    float rotation_speed = 1.0f; // radians/sec
    float time_accum = 0.0f;

    Entity();

    // Modify model matrix over time (rotate/translate/scale)
    void Update(float seconds_elapsed);

    // Render mesh triangles as wireframe (DDA)
    void RenderWireframe(Image& framebuffer, Camera& camera, const Color& color) const;

private:
    // Helpers
    static Vector2 ClipToScreen(const Vector3& clip_pos, int w, int h);
    static bool InsideClipCube(const Vector3& p);
};
