#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include "../../../common.h"
#include <SDL_video.h>
#include "../../../core/Memory.hpp"
#include "OpenGLFramebuffer.h"

constexpr GLuint BACKBUFFER_ID = 0;

OpenGLFramebuffer::OpenGLFramebuffer(SDL_Window * window)
{
    _id = BACKBUFFER_ID;
    _texture = 0;
    SDL_GetWindowSize(window, &_width, &_height);
}

OpenGLFramebuffer::OpenGLFramebuffer(sint32 width, sint32 height)
{
    _width = width;
    _height = height;

    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenFramebuffers(1, &_id);
    glBindFramebuffer(GL_FRAMEBUFFER, _id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
    if (_id != BACKBUFFER_ID)
    {
        glDeleteTextures(1, &_texture);
        glDeleteFramebuffers(1, &_id);
    }
}

void OpenGLFramebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, _id);
    glViewport(0, 0, (GLsizei)_width, (GLsizei)_height);
}

void * OpenGLFramebuffer::GetPixels() const
{
    void * pixels = Memory::Allocate<void>(_width * _height * 4);
    glReadPixels(0, 0, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Flip pixels vertically
    void * flippedPixels = Memory::Allocate<void>(_width * _height * 4);
    size_t stride = _width * 4;
    uint8 * src = (uint8 *)pixels + ((_height - 1) * stride);
    uint8 * dst = (uint8 *)flippedPixels;
    for (sint32 y = 0; y < _height; y++)
    {
        Memory::Copy(dst, src, stride);
        src -= stride;
        dst += stride;
    }
    Memory::Free(pixels);

    return flippedPixels;
}

#endif /* DISABLE_OPENGL */
