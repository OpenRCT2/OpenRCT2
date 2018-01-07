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

#include "ApplyTransparencyShader.h"

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

ApplyTransparencyShader::ApplyTransparencyShader() : OpenGLShaderProgram("applytransparency")
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
    glUniform1i(uOpaqueTex, 0);
    glUniform1i(uOpaqueDepth, 1);
    glUniform1i(uTransparentTex, 2);
    glUniform1i(uTransparentDepth, 3);
    glUniform1i(uPaletteTex, 4);
}

ApplyTransparencyShader::~ApplyTransparencyShader()
{
    glDeleteBuffers(1, &_vbo);
    glDeleteVertexArrays(1, &_vao);
}

void ApplyTransparencyShader::GetLocations()
{
    uOpaqueTex          = GetUniformLocation("uOpaqueTex");
    uOpaqueDepth        = GetUniformLocation("uOpaqueDepth");
    uTransparentTex     = GetUniformLocation("uTransparentTex");
    uTransparentDepth   = GetUniformLocation("uTransparentDepth");
    uPaletteTex         = GetUniformLocation("uPaletteTex");

    vPosition           = GetAttributeLocation("vPosition");
    vTextureCoordinate  = GetAttributeLocation("vTextureCoordinate");
}

void ApplyTransparencyShader::SetTextures(GLuint opaqueTex, GLuint opaqueDepth,
                                          GLuint transparentTex, GLuint transparentDepth,
                                          GLuint paletteTex)
{
    OpenGLAPI::SetTexture(0, GL_TEXTURE_2D, opaqueTex);
    OpenGLAPI::SetTexture(1, GL_TEXTURE_2D, opaqueDepth);
    OpenGLAPI::SetTexture(2, GL_TEXTURE_2D, transparentTex);
    OpenGLAPI::SetTexture(3, GL_TEXTURE_2D, transparentDepth);
    OpenGLAPI::SetTexture(4, GL_TEXTURE_RECTANGLE, paletteTex);
}

void ApplyTransparencyShader::Draw()
{
    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

#endif /* DISABLE_OPENGL */
