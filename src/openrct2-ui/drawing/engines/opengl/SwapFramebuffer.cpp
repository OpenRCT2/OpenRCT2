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
_opaqueFramebuffer(width, height), _transparentFramebuffer(width, height),
_finalFramebuffer(width, height, false)
{ }

void SwapFramebuffer::BindOpaque()
{
    _opaqueFramebuffer.Bind();
}

void SwapFramebuffer::BindTransparent()
{
    _transparentFramebuffer.Bind();
}

void SwapFramebuffer::ApplyTransparency(ApplyTransparencyShader &shader, GLuint paletteTex)
{
    _finalFramebuffer.Bind();
    glDisable(GL_DEPTH_TEST);
    shader.Use();
    shader.SetTextures(
        _opaqueFramebuffer.GetTexture(),
        _opaqueFramebuffer.GetDepthTexture(),
        _transparentFramebuffer.GetTexture(),
        _transparentFramebuffer.GetDepthTexture(),
        paletteTex
    );
    shader.Draw();

    //Change binding to guaruntee no undefined behavior
    _opaqueFramebuffer.Bind();
}

void SwapFramebuffer::Clear()
{
    static const GLfloat depthValue[1] = { 1.0f };
    static const GLuint indexValue[4] = { 0, 0, 0, 0 };

    _opaqueFramebuffer.Bind();
    glClearBufferfv(GL_DEPTH, 0, depthValue);

    _transparentFramebuffer.Bind();
    glClearBufferuiv(GL_COLOR, 0, indexValue);
    glClearBufferfv(GL_DEPTH, 0, depthValue);
}

#endif /* DISABLE_OPENGL */
