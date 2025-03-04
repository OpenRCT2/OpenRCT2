/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma optimize("", off)

#ifndef DISABLE_OPENGL

    #include "PostProcessShader.h"

using namespace OpenRCT2::Ui;

namespace
{
    struct VDStruct
    {
        GLfloat position[2];
        GLfloat texturecoordinate[2];
    };
} // namespace

constexpr VDStruct kVertexData[4] = {
    { -1.0f, -1.0f, 0.0f, 0.0f },
    { 1.0f, -1.0f, 1.0f, 0.0f },
    { -1.0f, 1.0f, 0.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f },
};

PostProcessShader::PostProcessShader(u8string_view name)
    : OpenGLShaderProgram(name)
{
    GetLocations();

    glCall(glGenBuffers, 1, &_vbo);
    glCall(glGenVertexArrays, 1, &_vao);

    glCall(glBindBuffer, GL_ARRAY_BUFFER, _vbo);
    glCall(glBufferData, GL_ARRAY_BUFFER, sizeof(kVertexData), kVertexData, GL_STATIC_DRAW);

    glCall(glBindVertexArray, _vao);
    glCall(
        glVertexAttribPointer, vPosition, 2, GL_FLOAT, GL_FALSE, GLsizei(sizeof(VDStruct)),
        reinterpret_cast<void*>(offsetof(VDStruct, position)));
    glCall(
        glVertexAttribPointer, vTextureCoordinate, 2, GL_FLOAT, GL_FALSE, GLsizei(sizeof(VDStruct)),
        reinterpret_cast<void*>(offsetof(VDStruct, texturecoordinate)));

    glCall(glEnableVertexAttribArray, vPosition);
    glCall(glEnableVertexAttribArray, vTextureCoordinate);

    Use();
    glCall(glUniform1i, uTexture, 0);
}

PostProcessShader::~PostProcessShader()
{
    glDeleteBuffers(1, &_vbo);
    glDeleteVertexArrays(1, &_vao);
}

void PostProcessShader::GetLocations()
{
    uTexture = GetUniformLocation("uTexture");
    uTicks = GetUniformLocation("uTicks");

    uZoom = GetUniformLocation("uZoom");
    vPosition = GetAttributeLocation("vPosition");
    vTextureCoordinate = GetAttributeLocation("vTextureCoordinate");
}

void PostProcessShader::SetTexture(GLuint texture)
{
    OpenGLAPI::SetTexture(0, GL_TEXTURE_2D, texture);
}

void PostProcessShader::Draw()
{
    glCall(glBindVertexArray, _vao);
    glCall(glDrawArrays, GL_TRIANGLE_STRIP, 0, 4);
}

void PostProcessShader::SetTickCount(uint32_t ticks)
{
    glCall(glUniform1f, uTicks, (float)ticks);
}

void PostProcessShader::SetZoom(float zoom)
{
    glCall(glUniform1f, uZoom, zoom);
}

#endif /* DISABLE_OPENGL */
