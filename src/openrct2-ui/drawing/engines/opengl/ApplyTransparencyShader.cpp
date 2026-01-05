/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
    glCall(glUniform1i, uOpaqueTex, 0);
    glCall(glUniform1i, uOpaqueDepth, 1);
    glCall(glUniform1i, uTransparentTex, 2);
    glCall(glUniform1i, uTransparentDepth, 3);
    glCall(glUniform1i, uPaletteTex, 4);
    glCall(glUniform1i, uBlendPaletteTex, 5);
}

ApplyTransparencyShader::~ApplyTransparencyShader()
{
    glCall(glDeleteBuffers, 1, &_vbo);
    glCall(glDeleteVertexArrays, 1, &_vao);
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
    glCall(glBindVertexArray, _vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

#endif /* DISABLE_OPENGL */
