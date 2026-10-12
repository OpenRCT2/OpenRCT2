/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_OPENGL

    #include "ApplyDropShadowShader.h"

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

ApplyDropShadowShader::ApplyDropShadowShader()
    : OpenGLShaderProgram("dropshadow")
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
    glCall(glUniform1i, uCameraDepthTex, 0);
    glCall(glUniform1i, uSunDepthTex, 1);
}

ApplyDropShadowShader::~ApplyDropShadowShader()
{
    glCall(glDeleteBuffers, 1, &_vbo);
    glCall(glDeleteVertexArrays, 1, &_vao);
}

void ApplyDropShadowShader::GetLocations()
{
    uCameraDepthTex = GetUniformLocation("uCameraDepthTex");
    uSunDepthTex = GetUniformLocation("uSunDepthTex");
    uSunViewPos = GetUniformLocation("uSunViewPos");
    uSunViewSize = GetUniformLocation("uSunViewSize");
    uSunRotation = GetUniformLocation("uSunRotation");
    uCameraRotation = GetUniformLocation("uCameraRotation");
    uSunFromLeft = GetUniformLocation("uSunFromLeft");
    uSunZoomScale = GetUniformLocation("uSunZoomScale");
    uCameraZoomScale = GetUniformLocation("uCameraZoomScale");
    uShadowStrength = GetUniformLocation("uShadowStrength");

    vPosition = GetAttributeLocation("vPosition");
    vTextureCoordinate = GetAttributeLocation("vTextureCoordinate");
}

void ApplyDropShadowShader::SetCameraDepthTexture(GLuint texture)
{
    OpenGLAPI::SetTexture(0, GL_TEXTURE_2D, texture);
}

void ApplyDropShadowShader::SetSunDepthTexture(GLuint texture)
{
    OpenGLAPI::SetTexture(1, GL_TEXTURE_2D, texture);
}

void ApplyDropShadowShader::SetParameters(
    float sunViewPosX, float sunViewPosY, float sunViewSizeX, float sunViewSizeY, int sunRotation, float sunZoomScale,
    float cameraZoomScale, float strength, int cameraRotation, bool sunFromLeft)
{
    glCall(glUniform1i, uCameraRotation, cameraRotation & 3);
    glCall(glUniform1i, uSunFromLeft, sunFromLeft ? 1 : 0);
    glCall(glUniform2f, uSunViewPos, sunViewPosX, sunViewPosY);
    glCall(glUniform2f, uSunViewSize, sunViewSizeX, sunViewSizeY);
    glCall(glUniform1i, uSunRotation, sunRotation);
    glCall(glUniform1f, uSunZoomScale, sunZoomScale);
    glCall(glUniform1f, uCameraZoomScale, cameraZoomScale);
    glCall(glUniform1f, uShadowStrength, strength);
}

void ApplyDropShadowShader::Draw()
{
    glCall(glBindVertexArray, _vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

#endif /* DISABLE_OPENGL */
