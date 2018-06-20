/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <string_view>
#include <tuple>
#include "../core/Imaging.h"
#include "Drawing.h"

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
            void * Buffer{};
            size_t BufferLength{};
        };

        enum class IMPORT_MODE
        {
            DEFAULT,
            CLOSEST,
            DITHERING,
        };

        enum IMPORT_FLAGS
        {
            NONE = 0,
            KEEP_PALETTE = 1 << 0,
            RLE = 1 << 1,
        };

        ImportResult Import(
            const Image& image,
            int32_t offsetX = 0,
            int32_t offsetY = 0,
            IMPORT_FLAGS flags = IMPORT_FLAGS::NONE,
            IMPORT_MODE mode = IMPORT_MODE::DEFAULT) const;

    private:
        static const PaletteBGRA StandardPalette[256];

        static std::vector<int32_t> GetPixels(const uint8_t * pixels, uint32_t width, uint32_t height, IMPORT_FLAGS flags, IMPORT_MODE mode);
        static std::tuple<void *, size_t> EncodeRaw(const int32_t * pixels, uint32_t width, uint32_t height);
        static std::tuple<void *, size_t> EncodeRLE(const int32_t * pixels, uint32_t width, uint32_t height);

        static int32_t CalculatePaletteIndex(IMPORT_MODE mode, int16_t * rgbaSrc, int32_t x, int32_t y, int32_t width, int32_t height);
        static int32_t GetPaletteIndex(const PaletteBGRA * palette, int16_t * colour);
        static bool IsTransparentPixel(const int16_t * colour);
        static bool IsChangablePixel(int32_t paletteIndex);
        static int32_t GetClosestPaletteIndex(const PaletteBGRA * palette, const int16_t * colour);
    };
}
