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

#include <unordered_map>
#include <SDL_pixels.h>
#include "../../../common.h"
#include "OpenGLAPI.h"

class TextureCache
{
private:
    std::unordered_map<uint32, GLuint> _imageTextureMap;
    SDL_Color _palette[256];

public:
    TextureCache();
    ~TextureCache();
    void SetPalette(const SDL_Color * palette);
    void InvalidateImage(uint32 image);
    GLuint GetOrLoadImageTexture(uint32 image);

private:
    GLuint LoadImageTexture(uint32 image);
    void * GetImageAsARGB(uint32 image, uint32 tertiaryColour, uint32 * outWidth, uint32 * outHeight);
    void FreeTextures();
};
