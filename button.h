//
//  button.h
//  ComputerGraphics
//
//  Created by GUILLEM on 20/1/26.
//
#pragma once

#include "image.h"
#include "framework.h"

// Button types to know what action to do
enum ButtonType
{
    BTN_PENCIL,
    BTN_ERASER,
    BTN_LINE,
    BTN_RECT,
    BTN_TRIANGLE,
    BTN_CLEAR,
    BTN_LOAD,
    BTN_SAVE,

    // colors
    BTN_COLOR_BLACK,
    BTN_COLOR_WHITE,
    BTN_COLOR_RED,
    BTN_COLOR_GREEN,
    BTN_COLOR_BLUE,
    BTN_COLOR_YELLOW,
    BTN_COLOR_CYAN,
    BTN_COLOR_PINK
};

class Button
{
    
public:
    
    Image icon;        // Image used to render the button
    Vector2 position;
    ButtonType type;   // Action id
    
    Button() {}
    Button(const Image& img, Vector2 pos, ButtonType t): icon(img), position(pos), type(t) {}
    
    // Check if mouse is inside button area
    bool IsMouseInside(Vector2 mousePosition) const;
    
    // Draw button icon on framebuffer
    void Render(Image& framebuffer) const;
};

