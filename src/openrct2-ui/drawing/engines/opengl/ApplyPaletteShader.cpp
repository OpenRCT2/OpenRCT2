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

#ifndef DISABLE_OPENGL

#include "ApplyPaletteShader.h"

namespace
{
    struct VDStruct
    {
        GLfloat position[2];
        GLfloat texturecoordinate[2];
    };
}

constexpr VDStruct VertexData[4] =
{
    { -1.0f, -1.0f, 0.0f, 0.0f },
    {  1.0f, -1.0f, 1.0f, 0.0f },
    { -1.0f,  1.0f, 0.0f, 1.0f },
    {  1.0f,  1.0f, 1.0f, 1.0f },
};

ApplyPaletteShader::ApplyPaletteShader() : OpenGLShaderProgram("applypalette")
{
    GetLocations();

    glGenBuffers(1, &_vbo);
    glGenVertexArrays(1, &_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData), VertexData, GL_STATIC_DRAW);

    glBindVertexArray(_vao);
    glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, sizeof(VDStruct), (void*) offsetof(VDStruct, position));
    glVertexAttribPointer(vTextureCoordinate, 2, GL_FLOAT, GL_FALSE, sizeof(VDStruct), (void*) offsetof(VDStruct, texturecoordinate));

    glEnableVertexAttribArray(vPosition);
    glEnableVertexAttribArray(vTextureCoordinate);

    Use();
    glUniform1i(uTexture, 0);
}

ApplyPaletteShader::~ApplyPaletteShader()
{
    glDeleteBuffers(1, &_vbo);
    glDeleteVertexArrays(1, &_vao);
}

void ApplyPaletteShader::GetLocations()
{
    uTexture            = GetUniformLocation("uTexture");
    uPalette            = GetUniformLocation("uPalette");

    vPosition           = GetAttributeLocation("vPosition");
    vTextureCoordinate  = GetAttributeLocation("vTextureCoordinate");
}

void ApplyPaletteShader::SetTexture(GLuint texture)
{
    OpenGLAPI::SetTexture(0, GL_TEXTURE_2D, texture);
}

void ApplyPaletteShader::SetPalette(const vec4 * glPalette)
{
    glUniform4fv(uPalette, 256, (const GLfloat *)glPalette);
}

void ApplyPaletteShader::Draw()
{
    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

#endif /* DISABLE_OPENGL */
