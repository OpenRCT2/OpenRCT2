/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Imaging.h"
#include "Drawing.h"

#include <string_view>
#include <tuple>

struct Image;

namespace OpenRCT2::Drawing
{
    /**
     * Imports images to the internal RCT G1 format.
     */
    class ImageImporter
    {
    public:
        struct ImportResult
        {
            rct_g1_element Element{};
            std::vector<uint8_t> Buffer;
        };

        enum class ImportMode : uint8_t
        {
            Default,
            Closest,
            Dithering,
        };

        enum ImportFlags : uint8_t
        {
            None = 0,
            RLE = (1 << 0),
            BMP = (1 << 1),
            KeepIndices = (1 << 2),
        };

        ImportResult Import(
            const Image& image, const GamePalette& spritePalette, int32_t srcX, int32_t srcY, int32_t width, int32_t height,
            int32_t offsetX, int32_t offsetY, ImportFlags flags = ImportFlags::None,
            ImportMode mode = ImportMode::Default) const;
        ImportResult Import(
            const Image& image, const GamePalette& spritePalette, int32_t offsetX = 0, int32_t offsetY = 0,
            ImportFlags flags = ImportFlags::None, ImportMode mode = ImportMode::Default) const;

    private:
        enum class PaletteIndexType : uint8_t
        {
            Normal,
            PrimaryRemap,
            SecondaryRemap,
            TertiaryRemap,
            Special,
        };

        static std::vector<int32_t> GetPixels(
            const uint8_t* pixels, uint32_t pitch, uint32_t srcX, uint32_t srcY, uint32_t width, uint32_t height,
            ImportFlags flags, ImportMode mode, const GamePalette& spritePalette);
        static std::vector<uint8_t> EncodeRaw(const int32_t* pixels, uint32_t width, uint32_t height);
        static std::vector<uint8_t> EncodeRLE(const int32_t* pixels, uint32_t width, uint32_t height);

        static int32_t CalculatePaletteIndex(
            ImportMode mode, int16_t* rgbaSrc, int32_t x, int32_t y, int32_t width, int32_t height, const GamePalette& palette);
        static int32_t GetPaletteIndex(const GamePalette& palette, int16_t* colour);
        static bool IsTransparentPixel(const int16_t* colour);
        static bool IsInPalette(const GamePalette& palette, int16_t* colour);
        static bool IsChangablePixel(int32_t paletteIndex);
        static PaletteIndexType GetPaletteIndexType(int32_t paletteIndex);
        static int32_t GetClosestPaletteIndex(const GamePalette& palette, const int16_t* colour);
    };
} // namespace OpenRCT2::Drawing
