#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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
#include "DrawCommands.h"

class DrawLineShader final : public OpenGLShaderProgram
{
private:
    GLuint uScreenSize;

    GLuint vClip;
    GLuint vBounds;
    GLuint vColour;
    GLuint vDepth;

    GLuint vVertMat;

    GLuint _vbo;
    GLuint _vboInstances;
    GLuint _vao;

public:
    DrawLineShader();
    ~DrawLineShader() override;

    void SetScreenSize(sint32 width, sint32 height);
    void DrawInstances(const LineCommandBatch& instances);

private:
    void GetLocations();
};
