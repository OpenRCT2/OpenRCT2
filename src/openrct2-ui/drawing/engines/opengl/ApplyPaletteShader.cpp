/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_OPENGL

#    include "ApplyPaletteShader.h"

namespace
{
    struct VDStruct
    {
        GLfloat position[2];
        GLfloat texturecoordinate[2];
    };
} // namespace

constexpr VDStruct VertexData[4] = {
    { -1.0f, -1.0f, 0.0f, 0.0f },
    { 1.0f, -1.0f, 1.0f, 0.0f },
    { -1.0f, 1.0f, 0.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f },
};

ApplyPaletteShader::ApplyPaletteShader()
    : OpenGLShaderProgram("applypalette")
{
    GetLocations();

    glGenBuffers(1, &_vbo);
    glGenVertexArrays(1, &_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData), VertexData, GL_STATIC_DRAW);

    glBindVertexArray(_vao);
    glVertexAttribPointer(
        vPosition, 2, GL_FLOAT, GL_FALSE, sizeof(VDStruct), reinterpret_cast<void*>(offsetof(VDStruct, position)));
    glVertexAttribPointer(
        vTextureCoordinate, 2, GL_FLOAT, GL_FALSE, sizeof(VDStruct),
        reinterpret_cast<void*>(offsetof(VDStruct, texturecoordinate)));

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
    uTexture = GetUniformLocation("uTexture");
    uPalette = GetUniformLocation("uPalette");

    vPosition = GetAttributeLocation("vPosition");
    vTextureCoordinate = GetAttributeLocation("vTextureCoordinate");
}

void ApplyPaletteShader::SetTexture(GLuint texture)
{
    OpenGLAPI::SetTexture(0, GL_TEXTURE_2D, texture);
}

void ApplyPaletteShader::SetPalette(const vec4* glPalette)
{
    glUniform4fv(uPalette, 256, reinterpret_cast<const GLfloat*>(glPalette));
}

void ApplyPaletteShader::Draw()
{
    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

#endif /* DISABLE_OPENGL */
