#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include "GLSLTypes.h"
#include "OpenGLShaderProgram.h"

class DrawLineShader final : public OpenGLShaderProgram
{
private:
    GLuint uScreenSize;
    GLuint uClip;
    GLuint uBounds;
    GLuint uColour;

    GLuint vIndex;

    GLuint _vbo;
    GLuint _vao;

public:
    DrawLineShader();
    ~DrawLineShader() override;

    void SetScreenSize(sint32 width, sint32 height);
    void SetClip(sint32 left, sint32 top, sint32 right, sint32 bottom);
    void SetBounds(sint32 x0, sint32 y0, sint32 x1, sint32 y1);
    void SetColour(vec4f colour);

    void Draw(sint32 x0, sint32 y0, sint32 x1, sint32 y1);

private:
    void GetLocations();
};
