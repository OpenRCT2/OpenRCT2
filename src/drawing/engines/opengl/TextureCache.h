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
#include <array>
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

// TODO: Handle no more slots remaining (allocate more atlases?)
// TODO: Handle images larger than 256x256

// This is the maximum width and height of each atlas (2048 -> 4 MB)
constexpr int TEXTURE_CACHE_MAX_ATLAS_SIZE = 2048;

// Location of an image (texture atlas index, slot and normalized coordinates)
struct CachedTextureInfo {
    GLuint index;
    GLuint slot;
    vec4i bounds;
    vec4f normalizedBounds;
};

// Represents a texture atlas that images of a given maximum size can be allocated from
// Atlases are all stored in the same 2D texture array, occupying the specified index
class Atlas {
private:
    GLuint _index;
    int _imageWidth, _imageHeight;
    int _atlasWidth, _atlasHeight;
    std::vector<GLuint> _freeSlots;

    int _cols, _rows;

public:
    Atlas(GLuint index, int imageWidth, int imageHeight) {
        _index = index;
        _imageWidth = imageWidth;
        _imageHeight = imageHeight;
    }

    void Initialise(int atlasWidth, int atlasHeight) {
        _atlasWidth = atlasWidth;
        _atlasHeight = atlasHeight;

        _cols = _atlasWidth / _imageWidth;
        _rows = _atlasHeight / _imageHeight;

        _freeSlots.resize(_cols * _rows);
        for (size_t i = 0; i < _freeSlots.size(); i++) {
            _freeSlots[i] = i;
        }
    }

    CachedTextureInfo Allocate(int actualWidth, int actualHeight) {
        assert(_freeSlots.size() > 0);

        GLuint slot = _freeSlots.back();
        _freeSlots.pop_back();

        auto bounds = GetSlotCoordinates(slot, actualWidth, actualHeight);

#ifdef DEBUG
        printf("texture atlas (%d, %d) has %d slots left\n", _imageWidth, _imageHeight, GetFreeSlots());
#endif

        return {_index, slot, bounds, NormalizeCoordinates(bounds)};
    }

    void Free(const CachedTextureInfo& info) {
        assert(_index == info.index);

        _freeSlots.push_back(info.slot);
    }

    bool SupportsImage(int actualWidth, int actualHeight) const {
        return actualWidth <= _imageWidth && actualHeight <= _imageHeight;
    }

    int GetFreeSlots() const {
        return (int) _freeSlots.size();
    }

private:
    vec4i GetSlotCoordinates(GLuint slot, int actualWidth, int actualHeight) const {
        int row = slot / _cols;
        int col = slot % _cols;

        return vec4i{
            _imageWidth * col,
            _imageHeight * row,
            _imageWidth * col + actualWidth,
            _imageHeight * row + actualHeight,
        };
    }

    vec4f NormalizeCoordinates(const vec4i& coords) const {
        return vec4f{
            coords.x / (float) _atlasWidth,
            coords.y / (float) _atlasHeight,
            coords.z / (float) _atlasWidth,
            coords.w / (float) _atlasHeight
        };
    }
};

class TextureCache
{
private:
    bool _atlasInitialised = false;

    GLuint _atlasTextureArray;

    // Atlases should be ordered from small to large image support
    std::array<Atlas, 4> _atlases = {
        Atlas{0, 32, 32},
        Atlas{1, 64, 64},
        Atlas{2, 128, 128},
        Atlas{3, 256, 256}
    };

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

    GLuint GetAtlasTextureArray();

private:
    void InitialiseAtlases();
    CachedTextureInfo LoadImageTexture(uint32 image);
    CachedTextureInfo LoadGlyphTexture(uint32 image, uint8 * palette);
    CachedTextureInfo AllocateFromAppropriateAtlas(int imageWidth, int imageHeight);
    void * GetImageAsARGB(uint32 image, uint32 tertiaryColour, uint32 * outWidth, uint32 * outHeight);
    rct_drawpixelinfo * GetImageAsDPI(uint32 image, uint32 tertiaryColour);
    void * GetGlyphAsARGB(uint32 image, uint8 * palette, uint32 * outWidth, uint32 * outHeight);
    rct_drawpixelinfo * GetGlyphAsDPI(uint32 image, uint8 * palette);
    void * ConvertDPIto32bpp(const rct_drawpixelinfo * dpi);
    void FreeTextures();

    static rct_drawpixelinfo * CreateDPI(sint32 width, sint32 height);
    static void DeleteDPI(rct_drawpixelinfo * dpi);
};
