/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_OPENGL

    #include "SwapFramebuffer.h"

    #include "OpenGLFramebuffer.h"

using namespace OpenRCT2::Ui;

constexpr GLfloat kDepthValue[1] = { 1.0f };
constexpr GLfloat kDepthValueTransparent[1] = { 0.0f };
constexpr GLuint kIndexValue[4] = { 0, 0, 0, 0 };

SwapFramebuffer::SwapFramebuffer(int32_t width, int32_t height)
    : _opaqueFramebuffer(width, height)
    , _transparentFramebuffer(width, height, true, true, true)
    , _mixFramebuffer(width, height, false)
    , _backDepth(OpenGLFramebuffer::CreateDepthTexture(width, height))
{
    _transparentFramebuffer.Bind();
    glClearBufferfv(GL_DEPTH, 0, kDepthValueTransparent);
}

SwapFramebuffer::~SwapFramebuffer()
{
    glDeleteTextures(1, &_backDepth);
}

void SwapFramebuffer::ApplyTransparency(ApplyTransparencyShader& shader, GLuint paletteTex, GLuint blendPaletteTex)
{
    _mixFramebuffer.Bind();
    glDisable(GL_DEPTH_TEST);
    shader.Use();
    shader.SetTextures(
        _opaqueFramebuffer.GetTexture(), _opaqueFramebuffer.GetDepthTexture(), _transparentFramebuffer.GetTexture(),
        _transparentFramebuffer.GetDepthTexture(), paletteTex, blendPaletteTex);
    shader.Draw();

    _backDepth = _transparentFramebuffer.SwapDepthTexture(_backDepth);

    // Clear transparency buffers
    _transparentFramebuffer.Bind();
    glClearBufferuiv(GL_COLOR, 0, kIndexValue);
    glClearBufferfv(GL_DEPTH, 0, kDepthValueTransparent);

    _opaqueFramebuffer.SwapColourBuffer(_mixFramebuffer);
    // Change binding to guarantee no undefined behavior
    _opaqueFramebuffer.Bind();
}

void SwapFramebuffer::Clear()
{
    _opaqueFramebuffer.Bind();
    glClearBufferfv(GL_DEPTH, 0, kDepthValue);
}

#endif /* DISABLE_OPENGL */
