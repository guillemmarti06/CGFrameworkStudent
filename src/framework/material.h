//
//  material.h
//  ComputerGraphics
//
//  Created by GUILLEM on 8/3/26.
//
#pragma once

#include "framework.h"
#include "shader.h"
#include "texture.h"

// Simple light for Lab 5
struct sLight
{
    Vector3 position;
    Vector3 intensity;

    sLight()
    {
        position = Vector3(0.0f, 5.0f, 0.0f);
        intensity = Vector3(1.0f, 1.0f, 1.0f);
    }
};

struct sUniformData
{
    Matrix44 model;
    Matrix44 viewprojection;

    Vector3 camera_position;
    Vector3 ambient_intensity;

    sLight light;

    int use_color_texture;
    int use_specular_texture;
    int use_normal_texture;
};

class Material
{
public:
    Shader* shader = nullptr;
    
    // Material textures
    Texture* color_texture = nullptr;
    Texture* normal_texture = nullptr;
    
    // Material coefficients
    Vector3 Ka;
    Vector3 Kd;
    Vector3 Ks;
    float shininess = 50.0f;
    
    Material();
    void Enable(const sUniformData& uniformData);
    void Disable();
};
