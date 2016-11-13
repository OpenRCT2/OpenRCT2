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

#include "CopyFramebufferShader.h"
#include "OpenGLFramebuffer.h"
#include "SwapFramebuffer.h"

SwapFramebuffer::SwapFramebuffer(sint32 width, sint32 height)
{
    _width = width;
    _height = height;
    _targetFramebufferIndex = 0;
    _framebuffer[0] = new OpenGLFramebuffer(width, height);
    _framebuffer[1] = new OpenGLFramebuffer(width, height);
    _targetFramebuffer = _framebuffer[0];

    _copyFramebufferShader = new CopyFramebufferShader();
    _copyFramebufferShader->Use();
    _copyFramebufferShader->SetScreenSize(_width, _height);
    _copyFramebufferShader->SetBounds(0, 0, _width, _height);
    _copyFramebufferShader->SetTextureCoordinates(0, 1, 1, 0);
}

SwapFramebuffer::~SwapFramebuffer()
{
    delete _framebuffer[0];
    delete _framebuffer[1];
    delete _copyFramebufferShader;
}

GLuint SwapFramebuffer::GetSourceTexture() const
{
    return _sourceFramebuffer->GetTexture();
}

void SwapFramebuffer::SwapCopy()
{
    _sourceFramebuffer = _targetFramebuffer;
    _targetFramebufferIndex = (_targetFramebufferIndex + 1) & 1;
    _targetFramebuffer = _framebuffer[_targetFramebufferIndex];
    _targetFramebuffer->Bind();

    _copyFramebufferShader->Use();
    _copyFramebufferShader->SetTexture(GetSourceTexture());
    _copyFramebufferShader->Draw();
}

void SwapFramebuffer::Bind()
{
    _targetFramebuffer->Bind();
}

#endif /* DISABLE_OPENGL */
