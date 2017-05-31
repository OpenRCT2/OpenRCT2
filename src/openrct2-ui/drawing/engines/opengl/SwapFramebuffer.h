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

#pragma once

#include <openrct2/common.h>
#include "OpenGLAPI.h"

class CopyFramebufferShader;
class OpenGLFramebuffer;

/**
 * Class to maintain two different framebuffers where the active framebuffer
 * will swap between the two, copying the other's pixels in the process for
 * performing pre-processing filters.
 *
 * When you need to bind the current frame to a shader, call SwapCopy and
 * then bind the value of GetSourceTexture to your shader.
 */
class SwapFramebuffer final
{
private:
    sint32              _width                  = 0;
    sint32              _height                 = 0;
    uint8               _targetFramebufferIndex = 0;
    OpenGLFramebuffer * _targetFramebuffer      = nullptr;
    OpenGLFramebuffer * _sourceFramebuffer      = nullptr;
    OpenGLFramebuffer * _framebuffer[2]         = { 0 };

    CopyFramebufferShader * _copyFramebufferShader = nullptr;

public:
    SwapFramebuffer(sint32 width, sint32 height);
    ~SwapFramebuffer();

    /**
     * Gets the current target framebuffer.
     */
    const OpenGLFramebuffer * GetTargetFramebuffer() const { return _targetFramebuffer; }

    /**
     * Gets the texture ID for the source framebuffer.
     */
    GLuint GetSourceTexture() const;

    /**
     * Swaps the target framebuffer, binds it and then draws the previous
     * framebuffer resulting in the two buffers matching and ready for
     * pre-processing.
     */
    void SwapCopy();

    /**
     * Binds the current target framebuffer.
     */
    void Bind();
};
