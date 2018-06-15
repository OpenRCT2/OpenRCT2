/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/common.h>
#include <openrct2/drawing/Drawing.h>
#include "OpenGLAPI.h"

#include <vector>

struct SDL_Window;

class OpenGLFramebuffer
{
private:
    GLuint _id;
    GLuint _texture;
    GLuint _depth;
    sint32 _width;
    sint32 _height;

public:
    explicit OpenGLFramebuffer(SDL_Window * window);
    OpenGLFramebuffer(sint32 width, sint32 height, bool depth = true, bool integer = true);
    ~OpenGLFramebuffer();

    OpenGLFramebuffer(const OpenGLFramebuffer &) = delete;
    OpenGLFramebuffer &operator=(const OpenGLFramebuffer &) = delete;

    GLuint GetWidth() const { return _width; }
    GLuint GetHeight() const { return _height; }
    GLuint GetTexture() const { return _texture; }
    GLuint GetDepthTexture() const { return _depth; }

    void Bind() const;
    void BindDraw() const;
    void BindRead() const;
    void GetPixels(rct_drawpixelinfo &dpi) const;

    void SwapColourBuffer(OpenGLFramebuffer &other);
    GLuint SwapDepthTexture(GLuint depth);
    void Copy(OpenGLFramebuffer &src, GLenum filter);

    static GLuint CreateDepthTexture(sint32 width, sint32 height);
};
