/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_OPENGL

#    include "TextureCache.h"

#    include <algorithm>
#    include <openrct2/drawing/Drawing.h>
#    include <openrct2/util/Util.h>
#    include <openrct2/world/Location.hpp>
#    include <stdexcept>
#    include <vector>

constexpr uint32_t UNUSED_INDEX = 0xFFFFFFFF;

TextureCache::TextureCache()
{
    std::fill(_indexMap.begin(), _indexMap.end(), UNUSED_INDEX);
}

TextureCache::~TextureCache()
{
    FreeTextures();
}

void TextureCache::InvalidateImage(ImageIndex image)
{
    unique_lock lock(_mutex);

    uint32_t index = _indexMap[image];
    if (index == UNUSED_INDEX)
        return;

    AtlasTextureInfo& elem = _textureCache.at(index);

    _atlases[elem.index].Free(elem);
    _indexMap[image] = UNUSED_INDEX;

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
BasicTextureInfo TextureCache::GetOrLoadImageTexture(ImageId imageId)
{
    uint32_t index;

    // Try to read cached texture first.
    {
        shared_lock lock(_mutex);

        index = _indexMap[imageId.GetIndex()];
        if (index != UNUSED_INDEX)
        {
            const auto& info = _textureCache[index];
            return {
                info.index,
                info.normalizedBounds,
            };
        }
    }

    // Load new texture.
    unique_lock lock(_mutex);

    index = static_cast<uint32_t>(_textureCache.size());

    AtlasTextureInfo info = LoadImageTexture(imageId);

    _textureCache.push_back(info);
    _indexMap[imageId.GetIndex()] = index;

    return info;
}

BasicTextureInfo TextureCache::GetOrLoadGlyphTexture(ImageId imageId, const PaletteMap& paletteMap)
{
    GlyphId glyphId{};
    glyphId.Image = imageId.GetIndex();

    // Try to read cached texture first.
    {
        shared_lock lock(_mutex);

        uint8_t glyphMap[8];
        for (uint8_t i = 0; i < 8; i++)
        {
            glyphMap[i] = paletteMap[i];
        }
        std::copy_n(glyphMap, sizeof(glyphId.Palette), reinterpret_cast<uint8_t*>(&glyphId.Palette));

        auto kvp = _glyphTextureMap.find(glyphId);
        if (kvp != _glyphTextureMap.end())
        {
            const auto& info = kvp->second;
            return {
                info.index,
                info.normalizedBounds,
            };
        }
    }

    // Load new texture.
    unique_lock lock(_mutex);

    auto cacheInfo = LoadGlyphTexture(imageId, paletteMap);
    auto it = _glyphTextureMap.insert(std::make_pair(glyphId, cacheInfo));

    return (*it.first).second;
}

BasicTextureInfo TextureCache::GetOrLoadBitmapTexture(ImageIndex image, const void* pixels, size_t width, size_t height)
{
    uint32_t index;

    // Try to read cached texture first.
    {
        shared_lock lock(_mutex);

        index = _indexMap[image];
        if (index != UNUSED_INDEX)
        {
            const auto& info = _textureCache[index];
            return {
                info.index,
                info.normalizedBounds,
            };
        }
    }

    // Load new texture.
    unique_lock lock(_mutex);

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
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_atlasesTextureDimensions);
        if (_atlasesTextureDimensions > TEXTURE_CACHE_MAX_ATLAS_SIZE)
        {
            _atlasesTextureDimensions = TEXTURE_CACHE_MAX_ATLAS_SIZE;
        }

        // Determine maximum number of atlases (minimum of size and array limit)
        glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &_atlasesTextureIndicesLimit);
        if (_atlasesTextureDimensions < _atlasesTextureIndicesLimit)
            _atlasesTextureIndicesLimit = _atlasesTextureDimensions;

        glGenTextures(1, &_atlasesTexture);
        glBindTexture(GL_TEXTURE_2D_ARRAY, _atlasesTexture);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glGenTextures(1, &_paletteTexture);
        glBindTexture(GL_TEXTURE_2D, _paletteTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        GeneratePaletteTexture();

        _initialized = true;
        _atlasesTextureIndices = 0;
        _atlasesTextureCapacity = 0;
    }
}

void TextureCache::GeneratePaletteTexture()
{
    static_assert(PALETTE_TO_G1_OFFSET_COUNT + 5 < 256, "Height of palette too large!");
    constexpr int32_t height = 256;
    constexpr int32_t width = height;
    rct_drawpixelinfo dpi = CreateDPI(width, height);

    // Init no-op palette
    for (int i = 0; i < width; ++i)
    {
        dpi.bits[i] = i;
    }

    for (int i = 0; i < PALETTE_TO_G1_OFFSET_COUNT; ++i)
    {
        GLint y = PaletteToY(static_cast<FilterPaletteID>(i));

        auto g1Index = GetPaletteG1Index(i);
        if (g1Index.has_value())
        {
            auto element = gfx_get_g1_element(g1Index.value());
            gfx_draw_sprite_software(&dpi, ImageId(g1Index.value()), { -element->x_offset, y - element->y_offset });
        }
    }

    glBindTexture(GL_TEXTURE_2D, _paletteTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, dpi.bits);
    DeleteDPI(dpi);
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
            glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, oldPixels.data());
        }

        // Initial capacity will be 12 which covers most cases of a fully visible park.
        _atlasesTextureCapacity = (_atlasesTextureCapacity + 6) << 1UL;

        glBindTexture(GL_TEXTURE_2D_ARRAY, _atlasesTexture);
        glTexImage3D(
            GL_TEXTURE_2D_ARRAY, 0, GL_R8UI, _atlasesTextureDimensions, _atlasesTextureDimensions, _atlasesTextureCapacity, 0,
            GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);

        // Restore old data
        if (!oldPixels.empty())
        {
            glTexSubImage3D(
                GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, _atlasesTextureDimensions, _atlasesTextureDimensions, _atlasesTextureIndices,
                GL_RED_INTEGER, GL_UNSIGNED_BYTE, oldPixels.data());
        }
    }

    _atlasesTextureIndices = newIndices;
}

AtlasTextureInfo TextureCache::LoadImageTexture(ImageId imageId)
{
    rct_drawpixelinfo dpi = GetImageAsDPI(ImageId(imageId.GetIndex()));

    auto cacheInfo = AllocateImage(dpi.width, dpi.height);
    cacheInfo.image = imageId.GetIndex();

    glBindTexture(GL_TEXTURE_2D_ARRAY, _atlasesTexture);
    glTexSubImage3D(
        GL_TEXTURE_2D_ARRAY, 0, cacheInfo.bounds.x, cacheInfo.bounds.y, cacheInfo.index, dpi.width, dpi.height, 1,
        GL_RED_INTEGER, GL_UNSIGNED_BYTE, dpi.bits);

    DeleteDPI(dpi);

    return cacheInfo;
}

AtlasTextureInfo TextureCache::LoadGlyphTexture(ImageId imageId, const PaletteMap& paletteMap)
{
    rct_drawpixelinfo dpi = GetGlyphAsDPI(imageId, paletteMap);

    auto cacheInfo = AllocateImage(dpi.width, dpi.height);
    cacheInfo.image = imageId.GetIndex();

    glBindTexture(GL_TEXTURE_2D_ARRAY, _atlasesTexture);
    glTexSubImage3D(
        GL_TEXTURE_2D_ARRAY, 0, cacheInfo.bounds.x, cacheInfo.bounds.y, cacheInfo.index, dpi.width, dpi.height, 1,
        GL_RED_INTEGER, GL_UNSIGNED_BYTE, dpi.bits);

    DeleteDPI(dpi);

    return cacheInfo;
}

AtlasTextureInfo TextureCache::LoadBitmapTexture(ImageIndex image, const void* pixels, size_t width, size_t height)
{
    auto cacheInfo = AllocateImage(int32_t(width), int32_t(height));
    cacheInfo.image = image;
    glBindTexture(GL_TEXTURE_2D_ARRAY, _atlasesTexture);
    glTexSubImage3D(
        GL_TEXTURE_2D_ARRAY, 0, cacheInfo.bounds.x, cacheInfo.bounds.y, cacheInfo.index, GLsizei(width), GLsizei(height), 1,
        GL_RED_INTEGER, GL_UNSIGNED_BYTE, reinterpret_cast<const GLvoid*>(pixels));
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

#    ifdef DEBUG
    log_verbose("new texture atlas #%d (size %d) allocated", atlasIndex, atlasSize);
#    endif

    _atlases.emplace_back(atlasIndex, atlasSize);
    _atlases.back().Initialise(_atlasesTextureDimensions, _atlasesTextureDimensions);

    // Enlarge texture array to support new atlas
    EnlargeAtlasesTexture(1);

    // And allocate from the new atlas
    return _atlases.back().Allocate(imageWidth, imageHeight);
}

rct_drawpixelinfo TextureCache::GetImageAsDPI(ImageId imageId)
{
    auto g1Element = gfx_get_g1_element(imageId);
    int32_t width = g1Element->width;
    int32_t height = g1Element->height;

    rct_drawpixelinfo dpi = CreateDPI(width, height);
    gfx_draw_sprite_software(&dpi, imageId, { -g1Element->x_offset, -g1Element->y_offset });
    return dpi;
}

rct_drawpixelinfo TextureCache::GetGlyphAsDPI(ImageId imageId, const PaletteMap& palette)
{
    auto g1Element = gfx_get_g1_element(imageId);
    int32_t width = g1Element->width;
    int32_t height = g1Element->height;

    rct_drawpixelinfo dpi = CreateDPI(width, height);

    const auto glyphCoords = ScreenCoordsXY{ -g1Element->x_offset, -g1Element->y_offset };
    gfx_draw_sprite_palette_set_software(&dpi, imageId, glyphCoords, palette);
    return dpi;
}

void TextureCache::FreeTextures()
{
    // Free array texture
    glDeleteTextures(1, &_atlasesTexture);
    _textureCache.clear();
    std::fill(_indexMap.begin(), _indexMap.end(), UNUSED_INDEX);
}

rct_drawpixelinfo TextureCache::CreateDPI(int32_t width, int32_t height)
{
    size_t numPixels = width * height;
    auto pixels8 = new uint8_t[numPixels];
    std::fill_n(pixels8, numPixels, 0);

    rct_drawpixelinfo dpi;
    dpi.bits = pixels8;
    dpi.pitch = 0;
    dpi.x = 0;
    dpi.y = 0;
    dpi.width = width;
    dpi.height = height;
    dpi.zoom_level = ZoomLevel{ 0 };
    return dpi;
}

void TextureCache::DeleteDPI(rct_drawpixelinfo dpi)
{
    delete[] dpi.bits;
}

GLuint TextureCache::GetAtlasesTexture()
{
    return _atlasesTexture;
}

GLuint TextureCache::GetPaletteTexture()
{
    return _paletteTexture;
}

GLint TextureCache::PaletteToY(FilterPaletteID palette)
{
    return palette > FilterPaletteID::PaletteWater ? EnumValue(palette) + 5 : EnumValue(palette) + 1;
}

#endif /* DISABLE_OPENGL */
