//
//  material.cpp
//  ComputerGraphics
//
//  Created by GUILLEM on 8/3/26.
//
#include "material.h"

Material::Material()
{
    // Simple default values
    Ka = Vector3(0.1f, 0.1f, 0.1f);
    Kd = Vector3(1.0f, 1.0f, 1.0f);
    Ks = Vector3(1.0f, 1.0f, 1.0f);
    shininess = 50.0f;
}

void Material::Enable()
{
    if (shader)
        shader->Enable();
}

void Material::Disable()
{
    if (shader)
        shader->Disable();
}
