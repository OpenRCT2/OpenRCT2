/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_OPENGL

    #include "ApplyTransparencyShader.h"

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

ApplyTransparencyShader::ApplyTransparencyShader()
    : OpenGLShaderProgram("applytransparency")
{
    GetLocations();

    glGenBuffers(1, &_vbo);
    glGenVertexArrays(1, &_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kVertexData), kVertexData, GL_STATIC_DRAW);

    glBindVertexArray(_vao);
    glVertexAttribPointer(
        vPosition, 2, GL_FLOAT, GL_FALSE, sizeof(VDStruct), reinterpret_cast<void*>(offsetof(VDStruct, position)));
    glVertexAttribPointer(
        vTextureCoordinate, 2, GL_FLOAT, GL_FALSE, sizeof(VDStruct),
        reinterpret_cast<void*>(offsetof(VDStruct, texturecoordinate)));

    glEnableVertexAttribArray(vPosition);
    glEnableVertexAttribArray(vTextureCoordinate);

    Use();
    glUniform1i(uOpaqueTex, 0);
    glUniform1i(uOpaqueDepth, 1);
    glUniform1i(uTransparentTex, 2);
    glUniform1i(uTransparentDepth, 3);
    glUniform1i(uPaletteTex, 4);
    glUniform1i(uBlendPaletteTex, 5);
}

ApplyTransparencyShader::~ApplyTransparencyShader()
{
    glDeleteBuffers(1, &_vbo);
    glDeleteVertexArrays(1, &_vao);
}

void ApplyTransparencyShader::GetLocations()
{
    uOpaqueTex = GetUniformLocation("uOpaqueTex");
    uOpaqueDepth = GetUniformLocation("uOpaqueDepth");
    uTransparentTex = GetUniformLocation("uTransparentTex");
    uTransparentDepth = GetUniformLocation("uTransparentDepth");
    uPaletteTex = GetUniformLocation("uPaletteTex");
    uBlendPaletteTex = GetUniformLocation("uBlendPaletteTex");

    vPosition = GetAttributeLocation("vPosition");
    vTextureCoordinate = GetAttributeLocation("vTextureCoordinate");
}

void ApplyTransparencyShader::SetTextures(
    GLuint opaqueTex, GLuint opaqueDepth, GLuint transparentTex, GLuint transparentDepth, GLuint paletteTex,
    GLuint blendPaletteTex)
{
    OpenGLAPI::SetTexture(0, GL_TEXTURE_2D, opaqueTex);
    OpenGLAPI::SetTexture(1, GL_TEXTURE_2D, opaqueDepth);
    OpenGLAPI::SetTexture(2, GL_TEXTURE_2D, transparentTex);
    OpenGLAPI::SetTexture(3, GL_TEXTURE_2D, transparentDepth);
    OpenGLAPI::SetTexture(4, GL_TEXTURE_2D, paletteTex);
    OpenGLAPI::SetTexture(5, GL_TEXTURE_2D, blendPaletteTex);
}

void ApplyTransparencyShader::Draw()
{
    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

#endif /* DISABLE_OPENGL */
