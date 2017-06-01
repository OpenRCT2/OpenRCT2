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

#pragma once

#include <openrct2/common.h>
#include "OpenGLAPI.h"

struct SDL_Window;

class OpenGLFramebuffer
{
private:
    GLuint _id;
    GLuint _texture;
    sint32 _width;
    sint32 _height;

public:
    explicit OpenGLFramebuffer(SDL_Window * window);
    OpenGLFramebuffer(sint32 width, sint32 height);
    ~OpenGLFramebuffer();

    GLuint GetWidth() const { return _width; }
    GLuint GetHeight() const { return _height; }
    GLuint GetTexture() const { return _texture; }

    void Bind() const;
    void * GetPixels() const;
};
