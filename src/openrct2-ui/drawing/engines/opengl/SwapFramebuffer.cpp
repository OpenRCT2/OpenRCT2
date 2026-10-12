/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_OPENGL

    #include "SwapFramebuffer.h"

    #include "ApplyTransparencyShader.h"
    #include "OpenGLFramebuffer.h"

    #include <algorithm>

using namespace OpenRCT2::Ui;

constexpr GLfloat kDepthValue[1] = { 1.0f };
constexpr GLfloat kDepthValueTransparent[1] = { 0.0f };
constexpr GLuint kIndexValue[4] = { 0, 0, 0, 0 };

SwapFramebuffer::SwapFramebuffer(int32_t width, int32_t height)
    : _opaqueFramebuffer(width, height, true, true, false, true)
    , _transparentFramebuffer(width, height, true, true, true)
    , _mixFramebuffer(width, height, false)
    , _backDepth(OpenGLFramebuffer::CreateDepthTexture(width, height, false))
{
    _transparentFramebuffer.Bind();
    glCall(glClearBufferfv, GL_DEPTH, 0, kDepthValueTransparent);
}

SwapFramebuffer::~SwapFramebuffer()
{
    glCall(glDeleteTextures, 1, &_backDepth);
}

void SwapFramebuffer::ApplyTransparency(ApplyTransparencyShader& shader, GLuint paletteTex, GLuint blendPaletteTex)
{
    _mixFramebuffer.Bind();
    glCall(glDisable, GL_DEPTH_TEST);
    shader.Use();
    shader.SetTextures(
        _opaqueFramebuffer.GetTexture(), _opaqueFramebuffer.GetDepthTexture(), _transparentFramebuffer.GetTexture(),
        _transparentFramebuffer.GetDepthTexture(), paletteTex, blendPaletteTex);
    shader.Draw();

    _backDepth = _transparentFramebuffer.SwapDepthTexture(_backDepth);

    // Clear transparency buffers
    _transparentFramebuffer.Bind();
    glCall(glClearBufferuiv, GL_COLOR, 0, kIndexValue);
    glCall(glClearBufferfv, GL_DEPTH, 0, kDepthValueTransparent);

    _opaqueFramebuffer.SwapColourBuffer(_mixFramebuffer);
    // Change binding to guarantee no undefined behavior
    _opaqueFramebuffer.Bind();
}

void SwapFramebuffer::Clear()
{
    _opaqueFramebuffer.Bind();
    glCall(glClearBufferfv, GL_DEPTH, 0, kDepthValue);
}

void SwapFramebuffer::ClearWorldDepth()
{
    _opaqueFramebuffer.Bind();
    _opaqueFramebuffer.ClearWorldDepth();
}

void SwapFramebuffer::ClearWorldDepthRect(int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    _opaqueFramebuffer.Bind();
    glCall(glEnable, GL_SCISSOR_TEST);
    // OpenGL scissor origin is bottom-left.
    const int32_t height = static_cast<int32_t>(_opaqueFramebuffer.GetHeight());
    const int32_t width = std::max(0, right - left);
    const int32_t rectHeight = std::max(0, bottom - top);
    glCall(glScissor, left, height - top - rectHeight, width, rectHeight);
    _opaqueFramebuffer.ClearWorldDepth();
    glCall(glDisable, GL_SCISSOR_TEST);
}

#endif /* DISABLE_OPENGL */
