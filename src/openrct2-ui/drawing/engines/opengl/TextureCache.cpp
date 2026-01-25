/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_OPENGL

    #include "TextureCache.h"

    #include <algorithm>
    #include <openrct2/Diagnostic.h>
    #include <openrct2/core/EnumUtils.hpp>
    #include <openrct2/drawing/BlendColourMap.h>
    #include <openrct2/drawing/Drawing.h>
    #include <openrct2/world/Location.hpp>
    #include <stdexcept>
    #include <vector>

using namespace OpenRCT2::Ui;
using namespace OpenRCT2::Drawing;

constexpr uint32_t kUnusedIndex = 0xFFFFFFFF;

TextureCache::TextureCache()
{
    std::fill(_indexMap.begin(), _indexMap.end(), kUnusedIndex);
}

TextureCache::~TextureCache()
{
    FreeTextures();
}

void TextureCache::InvalidateImage(ImageIndex image)
{
    uint32_t index = _indexMap[image];
    if (index == kUnusedIndex)
        return;

    AtlasTextureInfo& elem = _textureCache.at(index);

    _atlases[elem.index].Free(elem);
    _indexMap[image] = kUnusedIndex;

    if (index == _textureCache.size() - 1)
    {
        // Last element can be popped back.
        _textureCache.pop_back();
    }
    else
    {
        // Swap last element with element to erase and then pop back.
        AtlasTextureInfo& last = _textureCache.back();

        // Move last to current.
        elem = last;

        // Change index for moved element.
        _indexMap[last.image] = index;

        _textureCache.pop_back();
    }
}

// Note: for performance reasons, this returns a BasicTextureInfo over an AtlasTextureInfo (also to not expose the cache)
BasicTextureInfo TextureCache::GetOrLoadImageTexture(const ImageId imageId)
{
    uint32_t index;

    // Try to read cached texture first.
    {
        index = _indexMap[imageId.GetIndex()];
        if (index != kUnusedIndex)
        {
            const auto& info = _textureCache[index];
            return {
                info.index,
                info.coords,
            };
        }
    }

    // Load new texture.
    index = static_cast<uint32_t>(_textureCache.size());

    AtlasTextureInfo info = LoadImageTexture(imageId);

    _textureCache.push_back(info);
    _indexMap[imageId.GetIndex()] = index;

    return info;
}

BasicTextureInfo TextureCache::GetOrLoadGlyphTexture(const ImageId imageId, const PaletteMap& paletteMap)
{
    GlyphId glyphId{};
    glyphId.Image = imageId.GetIndex();

    // Try to read cached texture first.
    {
        PaletteIndex glyphMap[8];
        for (uint8_t i = 0; i < 8; i++)
        {
            glyphMap[i] = paletteMap[i];
        }
        std::copy_n(glyphMap, sizeof(glyphId.Palette), reinterpret_cast<PaletteIndex*>(&glyphId.Palette));

        auto kvp = _glyphTextureMap.find(glyphId);
        if (kvp != _glyphTextureMap.end())
        {
            const auto& info = kvp->second;
            return {
                info.index,
                info.coords,
            };
        }
    }

    // Load new texture.
    auto cacheInfo = LoadGlyphTexture(imageId, paletteMap);
    auto it = _glyphTextureMap.insert(std::make_pair(glyphId, cacheInfo));

    return (*it.first).second;
}

BasicTextureInfo TextureCache::GetOrLoadBitmapTexture(ImageIndex image, const void* pixels, size_t width, size_t height)
{
    uint32_t index;

    // Try to read cached texture first.
    {
        index = _indexMap[image];
        if (index != kUnusedIndex)
        {
            const auto& info = _textureCache[index];
            return {
                info.index,
                info.coords,
            };
        }
    }

    // Load new texture.
    index = uint32_t(_textureCache.size());

    AtlasTextureInfo info = LoadBitmapTexture(image, pixels, width, height);

    _textureCache.push_back(info);
    _indexMap[image] = index;

    return info;
}

void TextureCache::CreateTextures()
{
    if (!_initialized)
    {
        // Determine width and height to use for texture atlases
        glCall(glGetIntegerv, GL_MAX_TEXTURE_SIZE, &_atlasesTextureDimensions);
        if (_atlasesTextureDimensions > kTextureCacheMaxAtlasSize)
        {
            _atlasesTextureDimensions = kTextureCacheMaxAtlasSize;
        }

        // Determine maximum number of atlases (minimum of size and array limit)
        glCall(glGetIntegerv, GL_MAX_ARRAY_TEXTURE_LAYERS, &_atlasesTextureIndicesLimit);
        if (_atlasesTextureDimensions < _atlasesTextureIndicesLimit)
            _atlasesTextureIndicesLimit = _atlasesTextureDimensions;

        glCall(glGenTextures, 1, &_atlasesTexture);
        glCall(glBindTexture, GL_TEXTURE_2D_ARRAY, _atlasesTexture);
        glCall(glTexParameteri, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glCall(glTexParameteri, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glCall(glPixelStorei, GL_UNPACK_ALIGNMENT, 1);

        glCall(glGenTextures, 1, &_paletteTexture);
        glCall(glBindTexture, GL_TEXTURE_2D, _paletteTexture);
        glCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glCall(glPixelStorei, GL_UNPACK_ALIGNMENT, 1);
        GeneratePaletteTexture();

        auto blendArray = GetBlendColourMap();
        if (blendArray != nullptr)
        {
            glCall(glGenTextures, 1, &_blendPaletteTexture);
            glCall(glBindTexture, GL_TEXTURE_2D, _blendPaletteTexture);
            glCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glCall(glPixelStorei, GL_UNPACK_ALIGNMENT, 1);
            glCall(
                glTexImage2D, GL_TEXTURE_2D, 0, GL_R8UI, kGamePaletteSize, kGamePaletteSize, 0, GL_RED_INTEGER,
                GL_UNSIGNED_BYTE, blendArray);
        }

        _initialized = true;
        _atlasesTextureIndices = 0;
        _atlasesTextureCapacity = 0;
    }
}

void TextureCache::GeneratePaletteTexture()
{
    static_assert(kPaletteTotalOffsets + 5 < 256, "Height of palette too large!");
    constexpr int32_t height = 256;
    constexpr int32_t width = height;
    RenderTarget rt = CreateRT(width, height);

    // Init no-op palette
    for (int i = 0; i < width; ++i)
    {
        rt.bits[i] = i;
    }

    for (int i = 0; i < kPaletteTotalOffsets; ++i)
    {
        auto filterPaletteId = static_cast<FilterPaletteID>(i);
        GLint y = PaletteToY(filterPaletteId);

        auto g1Index = GetPaletteG1Index(filterPaletteId);
        if (g1Index.has_value())
        {
            const auto* element = GfxGetG1Element(g1Index.value());
            if (element != nullptr)
            {
                GfxDrawSpriteSoftware(rt, ImageId(g1Index.value()), { -element->xOffset, y - element->yOffset });
            }
        }
    }

    glCall(glBindTexture, GL_TEXTURE_2D, _paletteTexture);
    glCall(glTexImage2D, GL_TEXTURE_2D, 0, GL_R8UI, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, rt.bits);
    DeleteRT(rt);
}

void TextureCache::EnlargeAtlasesTexture(GLuint newEntries)
{
    CreateTextures();

    GLuint newIndices = _atlasesTextureIndices + newEntries;

    std::vector<char> oldPixels;

    if (newIndices > _atlasesTextureCapacity)
    {
        // Retrieve current array data, growing buffer.
        oldPixels.resize(_atlasesTextureDimensions * _atlasesTextureDimensions * _atlasesTextureCapacity);
        if (!oldPixels.empty())
        {
            glCall(glGetTexImage, GL_TEXTURE_2D_ARRAY, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, oldPixels.data());
        }

        // Initial capacity will be 12 which covers most cases of a fully visible park.
        _atlasesTextureCapacity = (_atlasesTextureCapacity + 6) << 1uL;

        glCall(glBindTexture, GL_TEXTURE_2D_ARRAY, _atlasesTexture);
        glCall(
            glTexImage3D, GL_TEXTURE_2D_ARRAY, 0, GL_R8UI, _atlasesTextureDimensions, _atlasesTextureDimensions,
            _atlasesTextureCapacity, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);

        // Restore old data
        if (!oldPixels.empty())
        {
            glCall(
                glTexSubImage3D, GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, _atlasesTextureDimensions, _atlasesTextureDimensions,
                _atlasesTextureIndices, GL_RED_INTEGER, GL_UNSIGNED_BYTE, oldPixels.data());
        }
    }

    _atlasesTextureIndices = newIndices;
}

AtlasTextureInfo TextureCache::LoadImageTexture(const ImageId imageId)
{
    RenderTarget rt = GetImageAsRT(ImageId(imageId.GetIndex()));

    auto cacheInfo = AllocateImage(rt.width, rt.height);
    cacheInfo.image = imageId.GetIndex();

    glCall(glBindTexture, GL_TEXTURE_2D_ARRAY, _atlasesTexture);
    glCall(
        glTexSubImage3D, GL_TEXTURE_2D_ARRAY, 0, cacheInfo.bounds.x, cacheInfo.bounds.y, cacheInfo.index, rt.width, rt.height,
        1, GL_RED_INTEGER, GL_UNSIGNED_BYTE, rt.bits);

    DeleteRT(rt);

    return cacheInfo;
}

AtlasTextureInfo TextureCache::LoadGlyphTexture(const ImageId imageId, const PaletteMap& paletteMap)
{
    RenderTarget rt = GetGlyphAsRT(imageId, paletteMap);

    auto cacheInfo = AllocateImage(rt.width, rt.height);
    cacheInfo.image = imageId.GetIndex();

    glCall(glBindTexture, GL_TEXTURE_2D_ARRAY, _atlasesTexture);
    glCall(
        glTexSubImage3D, GL_TEXTURE_2D_ARRAY, 0, cacheInfo.bounds.x, cacheInfo.bounds.y, cacheInfo.index, rt.width, rt.height,
        1, GL_RED_INTEGER, GL_UNSIGNED_BYTE, rt.bits);

    DeleteRT(rt);

    return cacheInfo;
}

AtlasTextureInfo TextureCache::LoadBitmapTexture(ImageIndex image, const void* pixels, size_t width, size_t height)
{
    auto cacheInfo = AllocateImage(int32_t(width), int32_t(height));
    cacheInfo.image = image;
    glCall(glBindTexture, GL_TEXTURE_2D_ARRAY, _atlasesTexture);
    glCall(
        glTexSubImage3D, GL_TEXTURE_2D_ARRAY, 0, cacheInfo.bounds.x, cacheInfo.bounds.y, cacheInfo.index, GLsizei(width),
        GLsizei(height), 1, GL_RED_INTEGER, GL_UNSIGNED_BYTE, reinterpret_cast<const GLvoid*>(pixels));
    return cacheInfo;
}

AtlasTextureInfo TextureCache::AllocateImage(int32_t imageWidth, int32_t imageHeight)
{
    CreateTextures();

    // Find an atlas that fits this image
    for (Atlas& atlas : _atlases)
    {
        if (atlas.GetFreeSlots() > 0 && atlas.IsImageSuitable(imageWidth, imageHeight))
        {
            return atlas.Allocate(imageWidth, imageHeight);
        }
    }

    // If there is no such atlas, then create a new one
    if (static_cast<int32_t>(_atlases.size()) >= _atlasesTextureIndicesLimit)
    {
        throw std::runtime_error("more texture atlases required, but device limit reached!");
    }

    auto atlasIndex = static_cast<int32_t>(_atlases.size());
    int32_t atlasSize = powf(2, static_cast<float>(Atlas::CalculateImageSizeOrder(imageWidth, imageHeight)));

    #ifdef DEBUG
    LOG_VERBOSE("new texture atlas #%d (size %d) allocated", atlasIndex, atlasSize);
    #endif

    _atlases.emplace_back(atlasIndex, atlasSize);
    _atlases.back().Initialise(_atlasesTextureDimensions, _atlasesTextureDimensions);

    // Enlarge texture array to support new atlas
    EnlargeAtlasesTexture(1);

    // And allocate from the new atlas
    return _atlases.back().Allocate(imageWidth, imageHeight);
}

RenderTarget TextureCache::GetImageAsRT(const ImageId imageId)
{
    auto g1Element = GfxGetG1Element(imageId);
    int32_t width = g1Element->width;
    int32_t height = g1Element->height;

    RenderTarget rt = CreateRT(width, height);
    GfxDrawSpriteSoftware(rt, imageId, { -g1Element->xOffset, -g1Element->yOffset });
    return rt;
}

RenderTarget TextureCache::GetGlyphAsRT(const ImageId imageId, const PaletteMap& palette)
{
    auto g1Element = GfxGetG1Element(imageId);
    int32_t width = g1Element->width;
    int32_t height = g1Element->height;

    RenderTarget rt = CreateRT(width, height);

    const auto glyphCoords = ScreenCoordsXY{ -g1Element->xOffset, -g1Element->yOffset };
    GfxDrawSpritePaletteSetSoftware(rt, imageId, glyphCoords, palette);
    return rt;
}

void TextureCache::FreeTextures()
{
    // Free array texture
    glCall(glDeleteTextures, 1, &_atlasesTexture);
    _textureCache.clear();
    std::fill(_indexMap.begin(), _indexMap.end(), kUnusedIndex);
}

RenderTarget TextureCache::CreateRT(int32_t width, int32_t height)
{
    size_t numPixels = width * height;
    auto pixels8 = new uint8_t[numPixels];
    std::fill_n(pixels8, numPixels, 0);

    RenderTarget rt;
    rt.bits = pixels8;
    rt.pitch = 0;
    rt.x = 0;
    rt.y = 0;
    rt.width = width;
    rt.height = height;
    rt.zoom_level = ZoomLevel{ 0 };
    return rt;
}

void TextureCache::DeleteRT(RenderTarget rt)
{
    delete[] rt.bits;
}

GLuint TextureCache::GetAtlasesTexture()
{
    return _atlasesTexture;
}

GLuint TextureCache::GetPaletteTexture()
{
    return _paletteTexture;
}

GLuint TextureCache::GetBlendPaletteTexture()
{
    return _blendPaletteTexture;
}

GLint TextureCache::PaletteToY(FilterPaletteID palette)
{
    return palette > FilterPaletteID::paletteWater ? EnumValue(palette) + 5 : EnumValue(palette) + 1;
}

#endif /* DISABLE_OPENGL */
