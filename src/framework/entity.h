//
//  entity.h
//  ComputerGraphics
//
//  Created by GUILLEM on 10/2/26.
//

#pragma once

#include "framework.h" // For Matrix44
#include "mesh.h"
#include "image.h"
#include "camera.h"
#include "shader.h"
#include "texture.h"

// Entity: a renderable object that has a mesh + a model matrix (T/R/S)
class Entity
{
public:
    Mesh* mesh;      // Geometry to render (loaded from OBJ, etc.)
    Matrix44 model;  // Model matrix (scale/rotate/translate)
    Image* texture = NULL;
    
    // Lab4
    Texture* gpu_texture = NULL;
    Shader* shader = NULL;
    
    // Variables to make each entity different (simple scene)
    Vector3 base_position;
    float base_scale;
    float speed;
    
    // enum class to choose the mode in which we render
    enum class eRenderMode { POINTCLOUD, WIREFRAME, TRIANGLES, TRIANGLES_INTERPOLATED };
    eRenderMode mode = eRenderMode::TRIANGLES_INTERPOLATED;

    // Lab3 toggles
    bool useTexture = true;     // T
    bool useZBuffer = true;     // Z
    bool interpolateUV = true;  // C

    Entity();
    ~Entity();
    
    // Old CPU renderer (Lab 3)
    void Render(Image* framebuffer, Camera* camera, FloatImage* zBuffer);

    // New GPU renderer (Lab 4)
    void Render(Camera* camera);
    
    void Update(float seconds_elapsed);
};
