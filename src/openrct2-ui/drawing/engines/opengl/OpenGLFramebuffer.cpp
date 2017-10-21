#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef DISABLE_OPENGL

#include <openrct2/common.h>
#include <SDL_video.h>
#include <openrct2/core/Memory.hpp>
#include "OpenGLFramebuffer.h"

constexpr GLuint BACKBUFFER_ID = 0;

OpenGLFramebuffer::OpenGLFramebuffer(SDL_Window * window)
{
    _id = BACKBUFFER_ID;
    _texture = 0;
    _depth = 0;
    SDL_GetWindowSize(window, &_width, &_height);
}

OpenGLFramebuffer::OpenGLFramebuffer(sint32 width, sint32 height, bool depth)
{
    _width = width;
    _height = height;

    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (depth)
    {
        glGenTextures(1, &_depth);
        glBindTexture(GL_TEXTURE_2D, _depth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
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
    glViewport(0, 0, (GLsizei)_width, (GLsizei)_height);
}

void OpenGLFramebuffer::BindDraw() const
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id);
}

void OpenGLFramebuffer::BindRead() const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, _id);
}

void OpenGLFramebuffer::GetPixels(rct_drawpixelinfo &dpi) const
{
    assert(dpi.width == _width && dpi.height == _height);

    uint8 * pixels = Memory::Allocate<uint8>(_width * _height);
    glReadPixels(0, 0, _width, _height, GL_RED_INTEGER, GL_UNSIGNED_BYTE, pixels);

    // Flip pixels vertically on copy
    uint8 * src = pixels + ((_height - 1) * _width);
    uint8 * dst = dpi.bits;
    for (sint32 y = 0; y < _height; y++)
    {
        Memory::Copy(dst, src, _width);
        src -= _width;
        dst += dpi.width + dpi.pitch;
    }
    Memory::Free(pixels);
}

#endif /* DISABLE_OPENGL */
