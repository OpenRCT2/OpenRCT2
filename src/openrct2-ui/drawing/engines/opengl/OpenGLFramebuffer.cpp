/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_OPENGL

#    include "OpenGLFramebuffer.h"

#    include <SDL_video.h>
#    include <algorithm>
#    include <memory>
#    include <openrct2/common.h>

constexpr GLuint BACKBUFFER_ID = 0;

OpenGLFramebuffer::OpenGLFramebuffer(SDL_Window* window)
{
    _id = BACKBUFFER_ID;
    _texture = 0;
    _depth = 0;
    SDL_GL_GetDrawableSize(window, &_width, &_height);
}

OpenGLFramebuffer::OpenGLFramebuffer(int32_t width, int32_t height, bool depth, bool integer)
{
    _width = width;
    _height = height;

    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    if (integer)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (depth)
    {
        _depth = CreateDepthTexture(width, height);
    }
    else
    {
        _depth = 0;
    }

    glGenFramebuffers(1, &_id);
    glBindFramebuffer(GL_FRAMEBUFFER, _id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth, 0);
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
    if (_id != BACKBUFFER_ID)
    {
        glDeleteTextures(1, &_texture);
        glDeleteTextures(1, &_depth);
        glDeleteFramebuffers(1, &_id);
    }
}

void OpenGLFramebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, _id);
    glViewport(0, 0, static_cast<GLsizei>(_width), static_cast<GLsizei>(_height));
}

void OpenGLFramebuffer::BindDraw() const
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id);
}

void OpenGLFramebuffer::BindRead() const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, _id);
}

void OpenGLFramebuffer::GetPixels(DrawPixelInfo& dpi) const
{
    assert(dpi.width == _width && dpi.height == _height);

    auto pixels = std::make_unique<uint8_t[]>(_width * _height);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, pixels.get());

    // Flip pixels vertically on copy
    uint8_t* src = pixels.get() + ((_height - 1) * _width);
    uint8_t* dst = dpi.bits;
    for (int32_t y = 0; y < _height; y++)
    {
        std::copy_n(src, _width, dst);
        src -= _width;
        dst += dpi.width + dpi.pitch;
    }
}

void OpenGLFramebuffer::SwapColourBuffer(OpenGLFramebuffer& other)
{
    std::swap(_texture, other._texture);

    glBindFramebuffer(GL_FRAMEBUFFER, _id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, other._id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, other._texture, 0);
}

GLuint OpenGLFramebuffer::SwapDepthTexture(GLuint depth)
{
    std::swap(_depth, depth);

    glBindFramebuffer(GL_FRAMEBUFFER, _id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth, 0);

    return depth;
}

void OpenGLFramebuffer::Copy(OpenGLFramebuffer& src, GLenum filter)
{
    BindDraw();
    src.BindRead();
    glBlitFramebuffer(0, 0, src.GetWidth(), src.GetHeight(), 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, filter);
    Bind();
}

GLuint OpenGLFramebuffer::CreateDepthTexture(int32_t width, int32_t height)
{
    GLuint depth;
    glGenTextures(1, &depth);
    glBindTexture(GL_TEXTURE_2D, depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    return depth;
}

#endif /* DISABLE_OPENGL */
