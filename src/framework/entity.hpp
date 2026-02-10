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

    bool animated = false;
    float rotation_speed = 1.0f; // radians per second

    Entity();

    // Call this every frame if you want the entity to move/rotate
    void Update(float dt);

    // Wireframe render using the camera projection (LAB 2 requirement)
    void RenderWireframe(Image& framebuffer, Camera& camera, const Color& color) const;

private:
    Vector2 NDCToScreen(const Vector3& ndc, int width, int height) const;
    bool IsInsideClipSpace(const Vector3& ndc) const;
};
