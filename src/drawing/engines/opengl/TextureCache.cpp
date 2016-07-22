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

#include <vector>
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
    InitializeArrayTexture();

    auto kvp = _imageTextureMap.find(image);
    if (kvp != _imageTextureMap.end())
    {
        _imageTextureMap.erase(kvp);
        _freeSlots.push_back(kvp->second.slot);
    }
}

CachedTextureInfo TextureCache::GetOrLoadImageTexture(uint32 image)
{
    InitializeArrayTexture();

    auto kvp = _imageTextureMap.find(image & 0x7FFFF);
    if (kvp != _imageTextureMap.end())
    {
        return kvp->second;
    }

    auto cacheInfo = LoadImageTexture(image);
    _imageTextureMap[image & 0x7FFFF] = cacheInfo;

    //printf("%d slots left\n", (int) _freeSlots.size());

    return cacheInfo;
}

CachedTextureInfo TextureCache::GetOrLoadGlyphTexture(uint32 image, uint8 * palette)
{
    InitializeArrayTexture();

    GlyphId glyphId;
    glyphId.Image = image;
    Memory::Copy<void>(&glyphId.Palette, palette, sizeof(glyphId.Palette));

    auto kvp = _glyphTextureMap.find(glyphId);
    if (kvp != _glyphTextureMap.end())
    {
        return kvp->second;
    }

    auto cacheInfo = LoadGlyphTexture(image, palette);
    _glyphTextureMap[glyphId] = cacheInfo;

    //printf("%d slots left\n", (int) _freeSlots.size());

    return cacheInfo;
}

void TextureCache::InitializeArrayTexture() {
    if (!_arrayTextureInitialized) {
        glGenTextures(1, &_arrayTexture);
        glBindTexture(GL_TEXTURE_2D_ARRAY, _arrayTexture);
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_R8UI, TEXTURE_CACHE_ARRAY_WIDTH, TEXTURE_CACHE_ARRAY_HEIGHT, TEXTURE_CACHE_ARRAY_DEPTH);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        _freeSlots.resize(TEXTURE_CACHE_ARRAY_DEPTH);
        for (size_t i = 0; i < _freeSlots.size(); i++) _freeSlots[i] = i;

        _arrayTextureInitialized = true;
    }
}

CachedTextureInfo TextureCache::LoadImageTexture(uint32 image)
{
    rct_drawpixelinfo * dpi = GetImageAsDPI(image, 0);

    GLuint slot = _freeSlots.back();
    _freeSlots.pop_back();

    glBindTexture(GL_TEXTURE_2D_ARRAY, _arrayTexture);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, slot, dpi->width, dpi->height, 1, GL_RED_INTEGER, GL_UNSIGNED_BYTE, dpi->bits);
    
    DeleteDPI(dpi);

    return{slot, {dpi->width / (float) TEXTURE_CACHE_ARRAY_WIDTH, dpi->height / (float) TEXTURE_CACHE_ARRAY_HEIGHT}};
}

CachedTextureInfo TextureCache::LoadGlyphTexture(uint32 image, uint8 * palette)
{
    rct_drawpixelinfo * dpi = GetGlyphAsDPI(image, palette);

    GLuint slot = _freeSlots.back();
    _freeSlots.pop_back();

    glBindTexture(GL_TEXTURE_2D_ARRAY, _arrayTexture);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, slot, dpi->width, dpi->height, 1, GL_RED_INTEGER, GL_UNSIGNED_BYTE, dpi->bits);

    DeleteDPI(dpi);

    return {slot, {dpi->width / (float) TEXTURE_CACHE_ARRAY_WIDTH, dpi->height / (float) TEXTURE_CACHE_ARRAY_HEIGHT}};
}

void * TextureCache::GetImageAsARGB(uint32 image, uint32 tertiaryColour, uint32 * outWidth, uint32 * outHeight)
{
    rct_g1_element * g1Element = gfx_get_g1_element(image & 0x7FFFF);
    sint32 width = g1Element->width;
    sint32 height = g1Element->height;

    rct_drawpixelinfo * dpi = CreateDPI(width, height);
    gfx_draw_sprite_software(dpi, image, -g1Element->x_offset, -g1Element->y_offset, tertiaryColour);
    void * pixels32 = ConvertDPIto32bpp(dpi);
    DeleteDPI(dpi);

    *outWidth = width;
    *outHeight = height;
    return pixels32;
}

rct_drawpixelinfo * TextureCache::GetImageAsDPI(uint32 image, uint32 tertiaryColour)
{
    rct_g1_element * g1Element = gfx_get_g1_element(image & 0x7FFFF);
    sint32 width = g1Element->width;
    sint32 height = g1Element->height;
    
    rct_drawpixelinfo * dpi = CreateDPI(width, height);
    gfx_draw_sprite_software(dpi, image, -g1Element->x_offset, -g1Element->y_offset, tertiaryColour);
    return dpi;
}

void * TextureCache::GetGlyphAsARGB(uint32 image, uint8 * palette, uint32 * outWidth, uint32 * outHeight)
{
    rct_g1_element * g1Element = gfx_get_g1_element(image & 0x7FFFF);
    sint32 width = g1Element->width;
    sint32 height = g1Element->height;

    rct_drawpixelinfo * dpi = CreateDPI(width, height);
    gfx_draw_sprite_palette_set_software(dpi, image, -g1Element->x_offset, -g1Element->y_offset, palette, nullptr);
    void * pixels32 = ConvertDPIto32bpp(dpi);
    DeleteDPI(dpi);

    *outWidth = width;
    *outHeight = height;
    return pixels32;
}

rct_drawpixelinfo * TextureCache::GetGlyphAsDPI(uint32 image, uint8 * palette)
{
    rct_g1_element * g1Element = gfx_get_g1_element(image & 0x7FFFF);
    sint32 width = g1Element->width;
    sint32 height = g1Element->height;
    
    rct_drawpixelinfo * dpi = CreateDPI(width, height);
    gfx_draw_sprite_palette_set_software(dpi, image, -g1Element->x_offset, -g1Element->y_offset, palette, nullptr);
    return dpi;
}

void * TextureCache::ConvertDPIto32bpp(const rct_drawpixelinfo * dpi)
{
    size_t numPixels = dpi->width * dpi->height;
    uint8 * pixels32 = Memory::Allocate<uint8>(numPixels * 4);
    uint8 * src = dpi->bits;
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
    return pixels32;
}

void TextureCache::FreeTextures()
{
    // Free array texture
    glDeleteTextures(1, &_arrayTexture);
}

rct_drawpixelinfo * TextureCache::CreateDPI(sint32 width, sint32 height)
{
    size_t numPixels = width * height;
    uint8 * pixels8 = Memory::Allocate<uint8>(numPixels);
    Memory::Set(pixels8, 0, numPixels);

    rct_drawpixelinfo * dpi = new rct_drawpixelinfo();
    dpi->bits = pixels8;
    dpi->pitch = 0;
    dpi->x = 0;
    dpi->y = 0;
    dpi->width = width;
    dpi->height = height;
    dpi->zoom_level = 0;
    return dpi;
}

void TextureCache::DeleteDPI(rct_drawpixelinfo* dpi)
{
    Memory::Free(dpi->bits);
    delete dpi;
}

GLuint TextureCache::GetArrayTexture() {
    return _arrayTexture;
}

#endif /* DISABLE_OPENGL */
