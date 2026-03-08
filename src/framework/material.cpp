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

void Material::Enable(const sUniformData& uniformData)
{
    if (!shader)
        return;

    shader->Enable();

    // Scene uniforms
    shader->SetMatrix44("u_model", uniformData.model);
    shader->SetMatrix44("u_viewprojection", uniformData.viewprojection);
    shader->SetVector3("u_camera_position", uniformData.camera_position);
    shader->SetVector3("u_ambient_intensity", uniformData.ambient_intensity);
    shader->SetVector3("u_light_position", uniformData.light.position);
    shader->SetVector3("u_light_intensity", uniformData.light.intensity);

    shader->SetInt("u_use_color_texture", uniformData.use_color_texture);
    shader->SetInt("u_use_specular_texture", uniformData.use_specular_texture);
    shader->SetInt("u_use_normal_texture", uniformData.use_normal_texture);

    // Material uniforms
    shader->SetVector3("u_ka", Ka);
    shader->SetVector3("u_kd", Kd);
    shader->SetVector3("u_ks", Ks);
    shader->SetFloat("u_shininess", shininess);

    // Material textures
    if (color_texture)
        shader->SetTexture("u_texture", color_texture);

    if (normal_texture)
        shader->SetTexture("u_normal_texture", normal_texture);
}
void Material::Disable()
{
    if (shader)
        shader->Disable();
}
