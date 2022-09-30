/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_OPENGL

#    include "SwapFramebuffer.h"

#    include "OpenGLFramebuffer.h"

constexpr GLfloat depthValue[1] = { 1.0f };
constexpr GLfloat depthValueTransparent[1] = { 0.0f };
constexpr GLuint indexValue[4] = { 0, 0, 0, 0 };

SwapFramebuffer::SwapFramebuffer(int32_t width, int32_t height)
    : _opaqueFramebuffer(width, height)
    , _transparentFramebuffer(width, height)
    , _mixFramebuffer(width, height, false)
    , _backDepth(OpenGLFramebuffer::CreateDepthTexture(width, height))
{
    _transparentFramebuffer.Bind();
    glClearBufferfv(GL_DEPTH, 0, depthValueTransparent);
}

SwapFramebuffer::~SwapFramebuffer()
{
    glDeleteTextures(1, &_backDepth);
}

void SwapFramebuffer::ApplyTransparency(ApplyTransparencyShader& shader, GLuint paletteTex)
{
    _mixFramebuffer.Bind();
    glDisable(GL_DEPTH_TEST);
    shader.Use();
    shader.SetTextures(
        _opaqueFramebuffer.GetTexture(), _opaqueFramebuffer.GetDepthTexture(), _transparentFramebuffer.GetTexture(),
        _transparentFramebuffer.GetDepthTexture(), paletteTex);
    shader.Draw();

    _backDepth = _transparentFramebuffer.SwapDepthTexture(_backDepth);

    // Clear transparency buffers
    _transparentFramebuffer.Bind();
    glClearBufferuiv(GL_COLOR, 0, indexValue);
    glClearBufferfv(GL_DEPTH, 0, depthValueTransparent);

    _opaqueFramebuffer.SwapColourBuffer(_mixFramebuffer);
    // Change binding to guarantee no undefined behavior
    _opaqueFramebuffer.Bind();
}

void SwapFramebuffer::Clear()
{
    _opaqueFramebuffer.Bind();
    glClearBufferfv(GL_DEPTH, 0, depthValue);
}

#endif /* DISABLE_OPENGL */
