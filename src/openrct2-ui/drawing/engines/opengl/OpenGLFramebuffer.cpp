/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_OPENGL

    #include "OpenGLFramebuffer.h"

    #include <SDL_video.h>
    #include <algorithm>
    #include <cassert>
    #include <memory>
    #include <openrct2/drawing/RenderTarget.h>

using namespace OpenRCT2::Ui;

constexpr GLuint kBackBufferID = 0;
// R=worldX, G=worldY, B=(mode<<16)|z, A=unused. mode 0 == no depth written here.
constexpr GLuint kWorldDepthClear[4] = { 0u, 0u, 0u, 0u };
constexpr GLfloat kDepthValue[1] = { 1.0f };

OpenGLFramebuffer::OpenGLFramebuffer(SDL_Window* window)
{
    _id = kBackBufferID;
    _texture = 0;
    _worldDepth = 0;
    _depth = 0;
    SDL_GL_GetDrawableSize(window, &_width, &_height);
}

OpenGLFramebuffer::OpenGLFramebuffer(
    int32_t width, int32_t height, bool depth, bool integer, bool word, bool worldDepth, bool depth16)
{
    _width = width;
    _height = height;

    glCall(glGenTextures, 1, &_texture);
    glCall(glBindTexture, GL_TEXTURE_2D, _texture);
    if (integer)
    {
        int internalFormat = word ? GL_R16UI : GL_R8UI;
        int type = word ? GL_UNSIGNED_SHORT : GL_UNSIGNED_BYTE;
        glCall(glTexImage2D, GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RED_INTEGER, type, nullptr);
    }
    else
    {
        glCall(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }
    glCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (worldDepth)
    {
        _worldDepth = CreateWorldDepthTexture(width, height);
    }
    else
    {
        _worldDepth = 0;
    }

    if (depth)
    {
        _depth = CreateDepthTexture(width, height, depth16);
    }
    else
    {
        _depth = 0;
    }

    glCall(glGenFramebuffers, 1, &_id);
    glCall(glBindFramebuffer, GL_FRAMEBUFFER, _id);
    glCall(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);

    if (_worldDepth != 0)
    {
        glCall(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _worldDepth, 0);
    }

    if (depth)
    {
        glCall(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth, 0);
    }

    // Verify completeness
    const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        const char* errorMsg = "Unknown error";
        switch (status)
        {
            case GL_FRAMEBUFFER_UNDEFINED:
                errorMsg = "Undefined";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                errorMsg = "Incomplete attachment";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                errorMsg = "Missing attachment";
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                errorMsg = "Unsupported combination";
                break;
        }
        LOG_ERROR("Framebuffer incomplete: (%u) %s", status, errorMsg);
        _complete = false;
    }

    if (_worldDepth != 0)
    {
        const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glCall(glDrawBuffers, 2, drawBuffers);
    }
    else
    {
        const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
        glCall(glDrawBuffers, 1, drawBuffers);
    }

    // Clear the framebuffer with typed clears (glClear is invalid on integer colour attachments).
    if (integer)
    {
        const GLuint zero[4] = { 0, 0, 0, 0 };
        glCall(glClearBufferuiv, GL_COLOR, 0, zero);
        if (_worldDepth != 0)
        {
            ClearWorldDepth();
        }
        if (depth)
        {
            glCall(glClearBufferfv, GL_DEPTH, 0, kDepthValue);
        }
    }
    else
    {
        glCall(glClearColor, 0, 0, 0, 0);
        if (depth)
        {
            glCall(glClearDepth, 1.0f);
            glCall(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        else
        {
            glCall(glClear, GL_COLOR_BUFFER_BIT);
        }
    }

    glCall(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
    if (_id != kBackBufferID)
    {
        glCall(glDeleteTextures, 1, &_texture);
        if (_worldDepth != 0)
        {
            glCall(glDeleteTextures, 1, &_worldDepth);
        }
        glCall(glDeleteTextures, 1, &_depth);
        glCall(glDeleteFramebuffers, 1, &_id);
    }
}

void OpenGLFramebuffer::Bind() const
{
    glCall(glBindFramebuffer, GL_FRAMEBUFFER, _id);
    glCall(glViewport, 0, 0, static_cast<GLsizei>(_width), static_cast<GLsizei>(_height));
    // Default framebuffer (id 0) cannot use GL_COLOR_ATTACHMENTi draw buffers.
    if (_id == kBackBufferID)
    {
        return;
    }
    if (_worldDepth != 0)
    {
        const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glCall(glDrawBuffers, 2, drawBuffers);
    }
    else
    {
        const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
        glCall(glDrawBuffers, 1, drawBuffers);
    }
}

void OpenGLFramebuffer::BindDraw() const
{
    glCall(glBindFramebuffer, GL_DRAW_FRAMEBUFFER, _id);
}

void OpenGLFramebuffer::BindRead() const
{
    glCall(glBindFramebuffer, GL_READ_FRAMEBUFFER, _id);
}

void OpenGLFramebuffer::SwapColourBuffer(OpenGLFramebuffer& other)
{
    std::swap(_texture, other._texture);

    glCall(glBindFramebuffer, GL_FRAMEBUFFER, _id);
    glCall(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);

    glCall(glBindFramebuffer, GL_FRAMEBUFFER, other._id);
    glCall(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, other._texture, 0);
}

GLuint OpenGLFramebuffer::SwapDepthTexture(GLuint depth)
{
    std::swap(_depth, depth);

    glCall(glBindFramebuffer, GL_FRAMEBUFFER, _id);
    glCall(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth, 0);

    return depth;
}

void OpenGLFramebuffer::Copy(OpenGLFramebuffer& src, GLenum filter)
{
    BindDraw();
    src.BindRead();
    glCall(glReadBuffer, GL_COLOR_ATTACHMENT0);
    glCall(glDrawBuffer, GL_COLOR_ATTACHMENT0);
    glCall(glBlitFramebuffer, 0, 0, src.GetWidth(), src.GetHeight(), 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, filter);
    Bind();
}

void OpenGLFramebuffer::BlitWorldDepth(
    const OpenGLFramebuffer& src, int32_t srcX, int32_t srcY, int32_t width, int32_t height, int32_t dstX, int32_t dstY)
{
    if (_worldDepth == 0 || src._worldDepth == 0 || width <= 0 || height <= 0)
    {
        return;
    }

    BindDraw();
    src.BindRead();
    glCall(glReadBuffer, GL_COLOR_ATTACHMENT1);
    glCall(glDrawBuffer, GL_COLOR_ATTACHMENT1);
    glCall(
        glBlitFramebuffer, srcX, srcY, srcX + width, srcY + height, dstX, dstY, dstX + width, dstY + height,
        GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glCall(glReadBuffer, GL_COLOR_ATTACHMENT0);
    Bind();
}

void OpenGLFramebuffer::ClearWorldDepth()
{
    if (_worldDepth == 0)
    {
        return;
    }
    glCall(glBindFramebuffer, GL_FRAMEBUFFER, _id);
    glCall(glClearBufferuiv, GL_COLOR, 1, kWorldDepthClear);
}

GLuint OpenGLFramebuffer::CreateDepthTexture(int32_t width, int32_t height, bool halfPrecision)
{
    GLuint depth;
    glCall(glGenTextures, 1, &depth);
    glCall(glBindTexture, GL_TEXTURE_2D, depth);
    glCall(
        glTexImage2D, GL_TEXTURE_2D, 0, halfPrecision ? GL_DEPTH_COMPONENT16 : GL_DEPTH_COMPONENT24, width, height, 0,
        GL_DEPTH_COMPONENT, halfPrecision ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, nullptr);
    glCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    glCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return depth;
}

GLuint OpenGLFramebuffer::CreateWorldDepthTexture(int32_t width, int32_t height)
{
    GLuint texture;
    glCall(glGenTextures, 1, &texture);
    glCall(glBindTexture, GL_TEXTURE_2D, texture);
    // RGBA16UI (8 B/texel, half of the old RGBA32UI): worldX, worldY, (mode<<13)|worldZ,
    // surface data. See the packing note at the top of drawrect.frag for why each field fits.
    // Shares the FBO with the R8UI colour attachment.
    glCall(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA16UI, width, height, 0, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, nullptr);
    glCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return texture;
}

void OpenGLFramebuffer::GetPixels(Drawing::RenderTarget& rt) const
{
    assert(rt.width == _width && rt.height == _height);

    auto pixels = std::make_unique<Drawing::PaletteIndex[]>(_width * _height);
    glCall(glBindTexture, GL_TEXTURE_2D, _texture);
    glCall(glPixelStorei, GL_PACK_ALIGNMENT, 1);
    glCall(glGetTexImage, GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, pixels.get());

    // Flip pixels vertically on copy
    Drawing::PaletteIndex* src = pixels.get() + ((_height - 1) * _width);
    Drawing::PaletteIndex* dst = rt.bits;
    for (int32_t y = 0; y < _height; y++)
    {
        std::copy_n(src, _width, dst);
        src -= _width;
        dst += rt.LineStride();
    }
}

void OpenGLFramebuffer::SetPixels(const Drawing::RenderTarget& rt)
{
    assert(rt.width == _width && rt.height == _height);

    auto pixels = std::make_unique<Drawing::PaletteIndex[]>(_width * _height);
    // Flip pixels vertically on copy
    Drawing::PaletteIndex* dst = pixels.get() + ((_height - 1) * _width);
    Drawing::PaletteIndex* src = rt.bits;
    for (int32_t y = 0; y < _height; y++)
    {
        std::copy_n(src, _width, dst);
        src += rt.width + rt.pitch;
        dst -= _width;
    }

    glCall(glBindTexture, GL_TEXTURE_2D, _texture);
    glCall(glPixelStorei, GL_UNPACK_ALIGNMENT, 1);
    glCall(glTexSubImage2D, GL_TEXTURE_2D, 0, 0, 0, _width, _height, GL_RED_INTEGER, GL_UNSIGNED_BYTE, pixels.get());
}

#endif /* DISABLE_OPENGL */
