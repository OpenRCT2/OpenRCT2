#pragma region Copyright (c) 2018 OpenRCT2 Developers
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
            sint32 offsetX = 0,
            sint32 offsetY = 0,
            IMPORT_FLAGS flags = IMPORT_FLAGS::NONE,
            IMPORT_MODE mode = IMPORT_MODE::DEFAULT) const;

    private:
        static const PaletteBGRA StandardPalette[256];

        static std::vector<sint32> GetPixels(const uint8 * pixels, uint32 width, uint32 height, IMPORT_FLAGS flags, IMPORT_MODE mode);
        static std::tuple<void *, size_t> EncodeRaw(const sint32 * pixels, uint32 width, uint32 height);
        static std::tuple<void *, size_t> EncodeRLE(const sint32 * pixels, uint32 width, uint32 height);

        static sint32 CalculatePaletteIndex(IMPORT_MODE mode, sint16 * rgbaSrc, sint32 x, sint32 y, sint32 width, sint32 height);
        static sint32 GetPaletteIndex(const PaletteBGRA * palette, sint16 * colour);
        static bool IsTransparentPixel(const sint16 * colour);
        static bool IsChangablePixel(sint32 paletteIndex);
        static sint32 GetClosestPaletteIndex(const PaletteBGRA * palette, const sint16 * colour);
    };
}
