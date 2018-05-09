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

        ImportResult Import(
            const Image& image,
            sint32 offsetX = 0,
            sint32 offsetY = 0,
            bool keepPalette = false,
            IMPORT_MODE mode = IMPORT_MODE::DEFAULT) const;

    private:
        static const PaletteBGRA StandardPalette[256];

        static sint32 GetPaletteIndex(const PaletteBGRA * palette, sint16 * colour);
        static bool IsTransparentPixel(const sint16 * colour);
        static bool IsChangablePixel(sint32 paletteIndex);
        static sint32 GetClosestPaletteIndex(const PaletteBGRA * palette, const sint16 * colour);
    };
}
