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

#include <algorithm>
#include <unordered_map>
#include <vector>
#include <SDL_pixels.h>
#include <openrct2/common.h>
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
            size_t hash = k.Image * 7;
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

// This is the maximum width and height of each atlas, basically the
// granularity at which new atlases are allocated (2048 -> 4 MB of VRAM)
constexpr sint32 TEXTURE_CACHE_MAX_ATLAS_SIZE = 2048;

// Pixel dimensions of smallest supported slots in texture atlases
// Must be a power of 2!
constexpr sint32 TEXTURE_CACHE_SMALLEST_SLOT = 32;

// Location of an image (texture atlas index, slot and normalized coordinates)
struct CachedTextureInfo
{
    GLuint index;
    GLuint slot;
    vec4i bounds;
    vec4f normalizedBounds;
};

// Represents a texture atlas that images of a given maximum size can be allocated from
// Atlases are all stored in the same 2D texture array, occupying the specified index
// Slots in atlases are always squares.
class Atlas final
{
private:
    GLuint _index       = 0;
    sint32 _imageSize   = 0;
    sint32 _atlasWidth  = 0;
    sint32 _atlasHeight = 0;
    std::vector<GLuint> _freeSlots;

    sint32 _cols = 0;
    sint32 _rows = 0;

public:
    Atlas(GLuint index, sint32 imageSize)
    {
        _index = index;
        _imageSize = imageSize;
    }

    void Initialise(sint32 atlasWidth, sint32 atlasHeight)
    {
        _atlasWidth = atlasWidth;
        _atlasHeight = atlasHeight;

        _cols = _atlasWidth / _imageSize;
        _rows = _atlasHeight / _imageSize;

        _freeSlots.resize(_cols * _rows);
        for (size_t i = 0; i < _freeSlots.size(); i++)
        {
            _freeSlots[i] = (GLuint)i;
        }
    }

    CachedTextureInfo Allocate(sint32 actualWidth, sint32 actualHeight)
    {
        assert(_freeSlots.size() > 0);

        GLuint slot = _freeSlots.back();
        _freeSlots.pop_back();

        auto bounds = GetSlotCoordinates(slot, actualWidth, actualHeight);

        return
        {
            _index,
            slot,
            bounds,
            NormalizeCoordinates(bounds)
        };
    }

    void Free(const CachedTextureInfo& info)
    {
        assert(_index == info.index);

        _freeSlots.push_back(info.slot);
    }

    // Checks if specified image would be tightly packed in this atlas
    // by checking if it is within the right power of 2 range
    bool IsImageSuitable(sint32 actualWidth, sint32 actualHeight) const
    {
        sint32 imageOrder = CalculateImageSizeOrder(actualWidth, actualHeight);
        sint32 atlasOrder = (sint32) log2(_imageSize);

        return imageOrder == atlasOrder;
    }

    sint32 GetFreeSlots() const
    {
        return (sint32) _freeSlots.size();
    }

    static sint32 CalculateImageSizeOrder(sint32 actualWidth, sint32 actualHeight)
    {
        sint32 actualSize = std::max(actualWidth, actualHeight);

        if (actualSize < TEXTURE_CACHE_SMALLEST_SLOT) {
            actualSize = TEXTURE_CACHE_SMALLEST_SLOT;
        }

        return (sint32) ceil(log2f((float) actualSize));
    }

private:
    vec4i GetSlotCoordinates(GLuint slot, sint32 actualWidth, sint32 actualHeight) const
    {
        sint32 row = slot / _cols;
        sint32 col = slot % _cols;

        return vec4i
        {
            _imageSize * col,
            _imageSize * row,
            _imageSize * col + actualWidth,
            _imageSize * row + actualHeight,
        };
    }

    vec4f NormalizeCoordinates(const vec4i& coords) const
    {
        return vec4f
        {
            coords.x / (float) _atlasWidth,
            coords.y / (float) _atlasHeight,
            coords.z / (float) _atlasWidth,
            coords.w / (float) _atlasHeight
        };
    }
};

class TextureCache final
{
private:
    bool _atlasesTextureInitialised = false;

    GLuint _atlasesTexture            = 0;
    GLint _atlasesTextureDimensions   = 0;
    GLuint _atlasesTextureIndices     = 0;
    GLint _atlasesTextureIndicesLimit = 0;
    std::vector<Atlas> _atlases;

    std::unordered_map<uint32, CachedTextureInfo> _imageTextureMap;
    std::unordered_map<GlyphId, CachedTextureInfo, GlyphId::Hash, GlyphId::Equal> _glyphTextureMap;
    std::unordered_map<uint32, CachedTextureInfo> _paletteTextureMap;

    SDL_Color _palette[256];

public:
    TextureCache() = default;
    ~TextureCache();
    void SetPalette(const SDL_Color * palette);
    void InvalidateImage(uint32 image);
    CachedTextureInfo GetOrLoadImageTexture(uint32 image);
    CachedTextureInfo GetOrLoadGlyphTexture(uint32 image, uint8 * palette);
    CachedTextureInfo GetOrLoadPaletteTexture(uint32 image, uint32 tertiaryColour, bool special);

    GLuint GetAtlasesTexture();

private:
    void CreateAtlasesTexture();
    void AllocateAtlasesTexture();
    void EnlargeAtlasesTexture(GLuint newEntries);
    CachedTextureInfo LoadImageTexture(uint32 image);
    CachedTextureInfo LoadGlyphTexture(uint32 image, uint8 * palette);
    CachedTextureInfo LoadPaletteTexture(uint32 image, uint32 tertiaryColour, bool special);
    CachedTextureInfo AllocateImage(sint32 imageWidth, sint32 imageHeight);
    void * GetImageAsARGB(uint32 image, uint32 tertiaryColour, uint32 * outWidth, uint32 * outHeight);
    rct_drawpixelinfo * GetImageAsDPI(uint32 image, uint32 tertiaryColour);
    void * GetGlyphAsARGB(uint32 image, uint8 * palette, uint32 * outWidth, uint32 * outHeight);
    rct_drawpixelinfo * GetGlyphAsDPI(uint32 image, uint8 * palette);
    void * ConvertDPIto32bpp(const rct_drawpixelinfo * dpi);
    void FreeTextures();

    static rct_drawpixelinfo * CreateDPI(sint32 width, sint32 height);
    static void DeleteDPI(rct_drawpixelinfo * dpi);
};
