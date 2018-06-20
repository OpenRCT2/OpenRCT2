/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <unordered_map>
#include <vector>
#include <array>
#include <SDL2/SDL_pixels.h>
#include <openrct2/common.h>
#include "OpenGLAPI.h"
#include "GLSLTypes.h"

struct rct_drawpixelinfo;

struct GlyphId
{
    uint32_t Image;
    uint64_t Palette;

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
constexpr int32_t TEXTURE_CACHE_MAX_ATLAS_SIZE = 2048;

// Pixel dimensions of smallest supported slots in texture atlases
// Must be a power of 2!
constexpr int32_t TEXTURE_CACHE_SMALLEST_SLOT = 32;

struct BasicTextureInfo
{
    GLuint index;
    vec4 normalizedBounds;
};

// Location of an image (texture atlas index, slot and normalized coordinates)
struct AtlasTextureInfo : public BasicTextureInfo
{
    GLuint slot;
    ivec4 bounds;
    uint32_t image;
};

// Represents a texture atlas that images of a given maximum size can be allocated from
// Atlases are all stored in the same 2D texture array, occupying the specified index
// Slots in atlases are always squares.
class Atlas final
{
private:
    GLuint _index       = 0;
    int32_t _imageSize   = 0;
    int32_t _atlasWidth  = 0;
    int32_t _atlasHeight = 0;
    std::vector<GLuint> _freeSlots;

    int32_t _cols = 0;
    int32_t _rows = 0;

public:
    Atlas(GLuint index, int32_t imageSize)
    {
        _index = index;
        _imageSize = imageSize;
    }

    void Initialise(int32_t atlasWidth, int32_t atlasHeight)
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

    AtlasTextureInfo Allocate(int32_t actualWidth, int32_t actualHeight)
    {
        assert(_freeSlots.size() > 0);

        GLuint slot = _freeSlots.back();
        _freeSlots.pop_back();

        auto bounds = GetSlotCoordinates(slot, actualWidth, actualHeight);

        AtlasTextureInfo info;
        info.index = _index;
        info.slot = slot;
        info.bounds = bounds;
        info.normalizedBounds = NormalizeCoordinates(bounds);

        return info;
    }

    void Free(const AtlasTextureInfo& info)
    {
        assert(_index == info.index);

        _freeSlots.push_back(info.slot);
    }

    // Checks if specified image would be tightly packed in this atlas
    // by checking if it is within the right power of 2 range
    bool IsImageSuitable(int32_t actualWidth, int32_t actualHeight) const
    {
        int32_t imageOrder = CalculateImageSizeOrder(actualWidth, actualHeight);
        int32_t atlasOrder = (int32_t) log2(_imageSize);

        return imageOrder == atlasOrder;
    }

    int32_t GetFreeSlots() const
    {
        return (int32_t) _freeSlots.size();
    }

    static int32_t CalculateImageSizeOrder(int32_t actualWidth, int32_t actualHeight)
    {
        int32_t actualSize = std::max(actualWidth, actualHeight);

        if (actualSize < TEXTURE_CACHE_SMALLEST_SLOT) {
            actualSize = TEXTURE_CACHE_SMALLEST_SLOT;
        }

        return (int32_t) ceil(log2f((float) actualSize));
    }

private:
    ivec4 GetSlotCoordinates(GLuint slot, int32_t actualWidth, int32_t actualHeight) const
    {
        int32_t row = slot / _cols;
        int32_t col = slot % _cols;

        return ivec4
        {
            _imageSize * col,
            _imageSize * row,
            _imageSize * col + actualWidth,
            _imageSize * row + actualHeight,
        };
    }

    vec4 NormalizeCoordinates(const ivec4& coords) const
    {
        return vec4
        {
            coords.x / (float) _atlasWidth,
            coords.y / (float) _atlasHeight,
            coords.z / (float) _atlasWidth,
            coords.w / (float) _atlasHeight,
        };
    }
};

class TextureCache final
{
private:
    bool _initialized                 = false;

    GLuint _atlasesTexture            = 0;
    GLint _atlasesTextureDimensions   = 0;
    GLuint _atlasesTextureIndices     = 0;
    GLint _atlasesTextureIndicesLimit = 0;
    std::vector<Atlas> _atlases;
    std::unordered_map<GlyphId, AtlasTextureInfo, GlyphId::Hash, GlyphId::Equal> _glyphTextureMap;
    std::vector<AtlasTextureInfo> _textureCache;
    std::array<uint32_t, 0x7FFFF> _indexMap;

    GLuint _paletteTexture            = 0;

public:
    TextureCache();
    ~TextureCache();
    void InvalidateImage(uint32_t image);
    BasicTextureInfo GetOrLoadImageTexture(uint32_t image);
    BasicTextureInfo GetOrLoadGlyphTexture(uint32_t image, uint8_t * palette);

    GLuint GetAtlasesTexture();
    GLuint GetPaletteTexture();
    static GLint PaletteToY(uint32_t palette);

private:
    void CreateTextures();
    void GeneratePaletteTexture();
    void EnlargeAtlasesTexture(GLuint newEntries);
    AtlasTextureInfo LoadImageTexture(uint32_t image);
    AtlasTextureInfo LoadGlyphTexture(uint32_t image, uint8_t * palette);
    AtlasTextureInfo AllocateImage(int32_t imageWidth, int32_t imageHeight);
    rct_drawpixelinfo GetImageAsDPI(uint32_t image, uint32_t tertiaryColour);
    rct_drawpixelinfo GetGlyphAsDPI(uint32_t image, uint8_t * palette);
    void FreeTextures();

    static rct_drawpixelinfo CreateDPI(int32_t width, int32_t height);
    static void DeleteDPI(rct_drawpixelinfo dpi);
};
