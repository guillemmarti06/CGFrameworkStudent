//
//  button.cpp
//  ComputerGraphics
//
//  Created by GUILLEM on 20/1/26.
//
#include "button.h"

// Implement the mouse check function
bool Button::IsMouseInside(Vector2 mousePosition) const
{
    return mousePosition.x >= position.x &&
           mousePosition.x < position.x + icon.width &&
           mousePosition.y >= position.y &&
           mousePosition.y < position.y + icon.height;
}

// Draw button icon using the function Image::DrawImage, just raw pixels
void Button::Render(Image& framebuffer) const
{
    framebuffer.DrawImage(icon, (int)position.x, (int)position.y);
}
