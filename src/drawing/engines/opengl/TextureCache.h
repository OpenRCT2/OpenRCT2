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
#include "GLSLTypes.h"

struct rct_drawpixelinfo;

struct GlyphId
{
    uint32 Image;
    uint64 Palette;

    struct Hash
    {
        size_t operator()(const GlyphId &k) const
        {
            size_t hash = 0x3154A85E;
            hash = k.Image * 7;
            hash += (k.Palette & 0xFFFFFFFF) * 13;
            hash += (k.Palette >> 32) * 23;
            return hash;
        }
    };
 
    struct Equal
    {
        bool operator()(const GlyphId &lhs, const GlyphId &rhs) const
        {
           return lhs.Image == rhs.Image &&
                  lhs.Palette == rhs.Palette;
        }
    };
};

// TODO: Derive from hardware limits instead
// TODO: Support > 64x64 images
constexpr int TEXTURE_CACHE_ATLAS_WIDTH = 8192;
constexpr int TEXTURE_CACHE_ATLAS_HEIGHT = 8192;
constexpr int TEXTURE_CACHE_MAX_IMAGE_WIDTH = 64;
constexpr int TEXTURE_CACHE_MAX_IMAGE_HEIGHT = 64;
constexpr int TEXTURE_CACHE_IMAGES_U = TEXTURE_CACHE_ATLAS_WIDTH / TEXTURE_CACHE_MAX_IMAGE_WIDTH;
constexpr int TEXTURE_CACHE_IMAGES_V = TEXTURE_CACHE_ATLAS_HEIGHT / TEXTURE_CACHE_MAX_IMAGE_HEIGHT;
constexpr int TEXTURE_CACHE_MAX_IMAGES = TEXTURE_CACHE_IMAGES_U * TEXTURE_CACHE_IMAGES_V;

struct CachedTextureInfo {
    GLuint slot;
    vec4f bounds;
};

class TextureCache
{
private:
    bool _atlasTextureInitialized = false;
    GLuint _atlasTexture;

    std::vector<GLuint> _freeSlots;

    std::unordered_map<uint32, CachedTextureInfo> _imageTextureMap;
    std::unordered_map<GlyphId, CachedTextureInfo, GlyphId::Hash, GlyphId::Equal> _glyphTextureMap;

    SDL_Color _palette[256];

public:
    TextureCache();
    ~TextureCache();
    void SetPalette(const SDL_Color * palette);
    void InvalidateImage(uint32 image);
    CachedTextureInfo GetOrLoadImageTexture(uint32 image);
    CachedTextureInfo GetOrLoadGlyphTexture(uint32 image, uint8 * palette);

    GLuint GetAtlasTexture();

private:
    void InitializeAtlasTexture();
    CachedTextureInfo LoadImageTexture(uint32 image);
    CachedTextureInfo LoadGlyphTexture(uint32 image, uint8 * palette);
    void * GetImageAsARGB(uint32 image, uint32 tertiaryColour, uint32 * outWidth, uint32 * outHeight);
    rct_drawpixelinfo * GetImageAsDPI(uint32 image, uint32 tertiaryColour);
    void * GetGlyphAsARGB(uint32 image, uint8 * palette, uint32 * outWidth, uint32 * outHeight);
    rct_drawpixelinfo * GetGlyphAsDPI(uint32 image, uint8 * palette);
    void * ConvertDPIto32bpp(const rct_drawpixelinfo * dpi);
    void FreeTextures();
    vec4i CalculateAtlasCoordinates(GLuint slot, int width, int height);
    vec4f ConvertToNormalizedCoordinates(vec4i coordinates);

    static rct_drawpixelinfo * CreateDPI(sint32 width, sint32 height);
    static void DeleteDPI(rct_drawpixelinfo * dpi);
};
