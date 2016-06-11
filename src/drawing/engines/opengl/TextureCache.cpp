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

#include "../../../core/Memory.hpp"
#include "TextureCache.h"

extern "C"
{
    #include "../../drawing.h"
}

TextureCache::TextureCache()
{
}

TextureCache::~TextureCache()
{
    FreeTextures();
}

void TextureCache::SetPalette(const SDL_Color * palette)
{
    Memory::CopyArray(_palette, palette, 256);
}

void TextureCache::InvalidateImage(uint32 image)
{
    auto kvp = _imageTextureMap.find(image);
    if (kvp != _imageTextureMap.end())
    {
        GLuint texture = kvp->second;
        glDeleteTextures(1, &texture);

        _imageTextureMap.erase(kvp);
    }
}

GLuint TextureCache::GetOrLoadImageTexture(uint32 image)
{
    auto kvp = _imageTextureMap.find(image);
    if (kvp != _imageTextureMap.end())
    {
        return kvp->second;
    }

    GLuint texture = LoadImageTexture(image);
    _imageTextureMap[image] = texture;

    // if ((_textures.size() % 100) == 0)
    // {
    //     printf("Textures: %d\n", _textures.size());
    // }

    return texture;
}

GLuint TextureCache::LoadImageTexture(uint32 image)
{
    GLuint texture;
    glGenTextures(1, &texture);

    uint32 width, height;
    void * pixels32 = GetImageAsARGB(image, 0, &width, &height);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels32);

    delete [] (uint8 *) pixels32;

    return texture;
}

void * TextureCache::GetImageAsARGB(uint32 image, uint32 tertiaryColour, uint32* outWidth, uint32* outHeight)
{
    int g1Id = image & 0x7FFFF;
    rct_g1_element * g1Element = gfx_get_g1_element(g1Id);

    uint32 width = (uint32)g1Element->width;
    uint32 height = (uint32)g1Element->height;

    size_t numPixels = width * height;
    uint8 * pixels8 = new uint8[numPixels];
    Memory::Set(pixels8, 0, numPixels);

    rct_drawpixelinfo dpi;
    dpi.bits = pixels8;
    dpi.pitch = 0;
    dpi.x = 0;
    dpi.y = 0;
    dpi.width = width;
    dpi.height = height;
    dpi.zoom_level = 0;
    gfx_draw_sprite_software(&dpi, image, -g1Element->x_offset, -g1Element->y_offset, tertiaryColour);

    uint8 * pixels32 = new uint8[width * height * 4];
    uint8 * src = pixels8;
    uint8 * dst = pixels32;
    for (size_t i = 0; i < numPixels; i++)
    {
        uint8 paletteIndex = *src++;
        if (paletteIndex == 0)
        {
            // Transparent
            *dst++ = 0;
            *dst++ = 0;
            *dst++ = 0;
            *dst++ = 0;
        }
        else
        {
            SDL_Color colour = _palette[paletteIndex];
            *dst++ = colour.r;
            *dst++ = colour.g;
            *dst++ = colour.b;
            *dst++ = colour.a;
        }
    }

    delete[] pixels8;

    *outWidth = width;
    *outHeight = height;
    return pixels32;
}

void TextureCache::FreeTextures()
{
    size_t numTextures = _imageTextureMap.size();
    auto textures = std::vector<GLuint>(numTextures);
    for (auto kvp : _imageTextureMap)
    {
        textures.push_back(kvp.second);
    }

    glDeleteTextures(textures.size(), textures.data());
}
