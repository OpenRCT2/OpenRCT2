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

#include "CopyFramebufferShader.h"
#include "OpenGLFramebuffer.h"
#include "SwapFramebuffer.h"

SwapFramebuffer::SwapFramebuffer(sint32 width, sint32 height) :
_width(width), _height(height),
_targetFramebuffer(width, height), _sourceFramebuffer(width, height)
{ }

GLuint SwapFramebuffer::GetSourceTexture() const
{
    return _sourceFramebuffer.GetTexture();
}

void SwapFramebuffer::SwapCopy()
{
    // no longer performs swap, TODO rename function
    // blit target's texture to source's texture
    _sourceFramebuffer.BindDraw();
    _targetFramebuffer.BindRead();
    glBlitFramebuffer(0, 0, _width, _height,
                      0, 0, _width, _height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
    _targetFramebuffer.Bind();
}

void SwapFramebuffer::Bind()
{
    _targetFramebuffer.Bind();
}

#endif /* DISABLE_OPENGL */
