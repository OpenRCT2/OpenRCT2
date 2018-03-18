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

class ApplyTransparencyShader final : public OpenGLShaderProgram
{
private:
    GLuint uOpaqueTex;
    GLuint uOpaqueDepth;
    GLuint uTransparentTex;
    GLuint uTransparentDepth;
    GLuint uPaletteTex;

    GLuint vPosition;
    GLuint vTextureCoordinate;

    GLuint _vbo;
    GLuint _vao;

public:
    ApplyTransparencyShader();
    ~ApplyTransparencyShader() override;

    void SetTextures(GLuint opaqueTex, GLuint opaqueDepth,
                     GLuint transparentTex, GLuint transparentDepth,
                     GLuint paletteTex);
    void Draw();

private:
    void GetLocations();
};
