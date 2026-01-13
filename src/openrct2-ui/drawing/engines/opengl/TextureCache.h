/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GLSLTypes.h"

#include <SDL_pixels.h>
#include <array>
#include <cassert>
#include <openrct2/SpriteIds.h>
#include <openrct2/drawing/DrawingLock.hpp>
#include <unordered_map>
#include <vector>

struct PaletteMap;

namespace OpenRCT2::Drawing
{
    enum class FilterPaletteID : int32_t;
    struct RenderTarget;
} // namespace OpenRCT2::Drawing

namespace OpenRCT2::Ui
{
    struct GlyphId
    {
        ImageIndex Image;
        uint64_t Palette;

        struct Hash
        {
            size_t operator()(const GlyphId& k) const
            {
                size_t hash = k.Image * 7;
                hash += (k.Palette & 0xFFFFFFFFuL) * 13;
                hash += (k.Palette >> 32uL) * 23;
                return hash;
            }
        };

        struct Equal
        {
            bool operator()(const GlyphId& lhs, const GlyphId& rhs) const
            {
                return lhs.Image == rhs.Image && lhs.Palette == rhs.Palette;
            }
        };
    };

    // This is the maximum width and height of each atlas, basically the
    // granularity at which new atlases are allocated (2048 -> 4 MB of VRAM)
    constexpr int32_t kTextureCacheMaxAtlasSize = 2048;

    // Pixel dimensions of smallest supported slots in texture atlases
    // Must be a power of 2!
    constexpr int32_t kTextureCacheSmallestSlot = 32;

    struct BasicTextureInfo
    {
        GLuint index;
        vec4 coords;
    };

    // Location of an image (texture atlas index, slot and normalized coordinates)
    struct AtlasTextureInfo : public BasicTextureInfo
    {
        GLuint slot;
        ivec4 bounds;
        ImageIndex image;
    };

    // Represents a texture atlas that images of a given maximum size can be allocated from
    // Atlases are all stored in the same 2D texture array, occupying the specified index
    // Slots in atlases are always squares.
    class Atlas final
    {
    private:
        GLuint _index = 0;
        int32_t _imageSize = 0;
        int32_t _atlasWidth = 0;
        int32_t _atlasHeight = 0;
        std::vector<GLuint> _freeSlots;

        int32_t _cols = 0;
        int32_t _rows = 0;

    public:
        Atlas(GLuint index, int32_t imageSize)
            : _index(index)
            , _imageSize(imageSize)
        {
        }

        void Initialise(int32_t atlasWidth, int32_t atlasHeight)
        {
            _atlasWidth = atlasWidth;
            _atlasHeight = atlasHeight;

            _cols = std::max(1, _atlasWidth / _imageSize);
            _rows = std::max(1, _atlasHeight / _imageSize);

            _freeSlots.resize(_cols * _rows);
            for (size_t i = 0; i < _freeSlots.size(); i++)
            {
                _freeSlots[i] = static_cast<GLuint>(i);
            }
        }

        AtlasTextureInfo Allocate(int32_t actualWidth, int32_t actualHeight)
        {
            assert(!_freeSlots.empty());

            GLuint slot = _freeSlots.back();
            _freeSlots.pop_back();

            auto bounds = GetSlotCoordinates(slot, actualWidth, actualHeight);

            AtlasTextureInfo info{};
            info.index = _index;
            info.slot = slot;
            info.bounds = bounds;
            info.coords = vec4{
                static_cast<float>(bounds.x),
                static_cast<float>(bounds.y),
                static_cast<float>(_atlasWidth),
                static_cast<float>(_atlasHeight),
            };

            return info;
        }

        void Free(const AtlasTextureInfo& info)
        {
            assert(_index == info.index);

            _freeSlots.push_back(info.slot);
        }

        // Checks if specified image would be tightly packed in this atlas
        // by checking if it is within the right power of 2 range
        [[nodiscard]] bool IsImageSuitable(int32_t actualWidth, int32_t actualHeight) const
        {
            int32_t imageOrder = CalculateImageSizeOrder(actualWidth, actualHeight);
            int32_t atlasOrder = log2(_imageSize);

            return imageOrder == atlasOrder;
        }

        [[nodiscard]] int32_t GetFreeSlots() const
        {
            return static_cast<int32_t>(_freeSlots.size());
        }

        static int32_t CalculateImageSizeOrder(int32_t actualWidth, int32_t actualHeight)
        {
            int32_t actualSize = std::max(actualWidth, actualHeight);

            if (actualSize < kTextureCacheSmallestSlot)
            {
                actualSize = kTextureCacheSmallestSlot;
            }

            return static_cast<int32_t>(ceil(log2f(static_cast<float>(actualSize))));
        }

    private:
        [[nodiscard]] ivec4 GetSlotCoordinates(GLuint slot, int32_t actualWidth, int32_t actualHeight) const
        {
            int32_t row = slot / _cols;
            int32_t col = slot % _cols;

            return ivec4{
                _imageSize * col,
                _imageSize * row,
                _imageSize * col + actualWidth,
                _imageSize * row + actualHeight,
            };
        }
    };

    class TextureCache final
    {
    private:
        bool _initialized = false;

        GLuint _atlasesTexture = 0;
        GLint _atlasesTextureDimensions = 0;
        GLuint _atlasesTextureCapacity = 0;
        GLuint _atlasesTextureIndices = 0;
        GLint _atlasesTextureIndicesLimit = 0;
        std::vector<Atlas> _atlases;
        std::unordered_map<GlyphId, AtlasTextureInfo, GlyphId::Hash, GlyphId::Equal> _glyphTextureMap;
        std::vector<AtlasTextureInfo> _textureCache;
        std::array<uint32_t, SPR_IMAGE_LIST_END> _indexMap;

        GLuint _paletteTexture = 0;
        GLuint _blendPaletteTexture = 0;

    public:
        TextureCache();
        ~TextureCache();
        void InvalidateImage(ImageIndex image);
        BasicTextureInfo GetOrLoadImageTexture(ImageId imageId);
        BasicTextureInfo GetOrLoadGlyphTexture(ImageId imageId, const PaletteMap& paletteMap);
        BasicTextureInfo GetOrLoadBitmapTexture(ImageIndex image, const void* pixels, size_t width, size_t height);

        GLuint GetAtlasesTexture();
        GLuint GetPaletteTexture();
        GLuint GetBlendPaletteTexture();
        static GLint PaletteToY(Drawing::FilterPaletteID palette);

    private:
        void CreateTextures();
        void GeneratePaletteTexture();
        void EnlargeAtlasesTexture(GLuint newEntries);
        AtlasTextureInfo LoadImageTexture(ImageId image);
        AtlasTextureInfo LoadGlyphTexture(ImageId image, const PaletteMap& paletteMap);
        AtlasTextureInfo AllocateImage(int32_t imageWidth, int32_t imageHeight);
        AtlasTextureInfo LoadBitmapTexture(ImageIndex image, const void* pixels, size_t width, size_t height);
        static Drawing::RenderTarget GetImageAsRT(ImageId imageId);
        static Drawing::RenderTarget GetGlyphAsRT(ImageId imageId, const PaletteMap& paletteMap);
        void FreeTextures();

        static Drawing::RenderTarget CreateRT(int32_t width, int32_t height);
        static void DeleteRT(Drawing::RenderTarget rt);
    };
} // namespace OpenRCT2::Ui
