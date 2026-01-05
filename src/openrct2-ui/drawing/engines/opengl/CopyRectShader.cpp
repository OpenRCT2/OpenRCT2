/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_OPENGL

    #include "CopyRectShader.h"

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

CopyRectShader::CopyRectShader()
    : OpenGLShaderProgram("copyrect")
{
    GetLocations();

    glCall(glGenBuffers, 1, &_vbo);
    glCall(glGenVertexArrays, 1, &_vao);

    glCall(glBindBuffer, GL_ARRAY_BUFFER, _vbo);
    glCall(glBufferData, GL_ARRAY_BUFFER, sizeof(kVertexData), kVertexData, GL_STATIC_DRAW);

    glCall(glBindVertexArray, _vao);
    glCall(
        glVertexAttribPointer, vPosition, 2, GL_FLOAT, GL_FALSE, glSizeOf<VDStruct>(),
        reinterpret_cast<void*>(offsetof(VDStruct, position)));
    glCall(
        glVertexAttribPointer, vTextureCoordinate, 2, GL_FLOAT, GL_FALSE, glSizeOf<VDStruct>(),
        reinterpret_cast<void*>(offsetof(VDStruct, texturecoordinate)));

    glCall(glEnableVertexAttribArray, vPosition);
    glCall(glEnableVertexAttribArray, vTextureCoordinate);

    Use();
    glCall(glUniform1i, uTexture, 0); // Bind texture to unit 0
}

CopyRectShader::~CopyRectShader()
{
    glCall(glDeleteBuffers, 1, &_vbo);
    glCall(glDeleteVertexArrays, 1, &_vao);
}

void CopyRectShader::GetLocations()
{
    uTexture = GetUniformLocation("uTexture");
    uSourceRect = GetUniformLocation("uSourceRect");
    uTextureSize = GetUniformLocation("uTextureSize");
    vPosition = GetAttributeLocation("vPosition");
    vTextureCoordinate = GetAttributeLocation("vTextureCoordinate");
}

void CopyRectShader::SetTexture(GLuint texture)
{
    OpenGLAPI::SetTexture(0, GL_TEXTURE_2D, texture);
}

void CopyRectShader::SetSourceRect(int32_t x, int32_t y, int32_t width, int32_t height)
{
    glUniform4f(
        uSourceRect, static_cast<GLfloat>(x), static_cast<GLfloat>(y), static_cast<GLfloat>(width),
        static_cast<GLfloat>(height));
}

void CopyRectShader::SetTextureSize(int32_t width, int32_t height)
{
    glUniform2f(uTextureSize, static_cast<GLfloat>(width), static_cast<GLfloat>(height));
}

void CopyRectShader::Draw()
{
    glCall(glBindVertexArray, _vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

#endif /* DISABLE_OPENGL */
