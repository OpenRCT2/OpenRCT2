/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/CallingConventions.h"
#include "../core/FlagHolder.hpp"

#include <memory>
#include <vector>

namespace OpenRCT2::Drawing
{
    struct BGRColour;
}

namespace OpenRCT2
{
    enum class G1Flag : uint8_t
    {
        hasTransparency, // Image data contains transparent pixels (0XFF) which will not be rendered
        one,
        hasRLECompression, // Image data is encoded using RCT2's form of run length encoding
        isPalette,         // Image data is a sequence of palette entries R8G8B8
        hasZoomSprite,     // Use a different sprite for higher zoom levels
        noZoomDraw,        // Does not get drawn at higher zoom levels (only zoom 0)
    };
    using G1Flags = FlagHolder<uint16_t, G1Flag>;

    struct G1Palette
    {
        Drawing::BGRColour* palette = nullptr;
        int16_t numColours = 0;
        int16_t pad1 = 0; // unused for palettes
        int16_t startIndex = 0;
        int16_t pad2 = 0; // unused for palettes
        G1Flags flags = { G1Flag::isPalette };
        int32_t pad3 = 0; // unused for palettes
    };

    struct G1Element
    {
        uint8_t* offset = nullptr;
        int16_t width = 0;
        int16_t height = 0;
        int16_t xOffset = 0;
        int16_t yOffset = 0;

        G1Flags flags = {};
        int32_t zoomedOffset = 0;

        const G1Palette* asPalette() const
        {
            if (!flags.has(G1Flag::isPalette))
                return nullptr;

            return reinterpret_cast<const G1Palette*>(this);
        }
    };
    static_assert(sizeof(G1Palette) == sizeof(G1Element));

#pragma pack(push, 1)
    struct G1Header
    {
        uint32_t numEntries = 0;
        uint32_t totalSize = 0;
    };
    static_assert(sizeof(G1Header) == 8);
#pragma pack(pop)

    struct Gx
    {
        G1Header header;
        std::vector<G1Element> elements;
        std::unique_ptr<uint8_t[]> data;
    };

    struct StoredG1Element
    {
        uint32_t offset;       // 0x00 note: uint32_t always!
        int16_t width;         // 0x04
        int16_t height;        // 0x06
        int16_t xOffset;       // 0x08
        int16_t yOffset;       // 0x0A
        G1Flags flags;         // 0x0C
        uint16_t zoomedOffset; // 0x0E
    };
    static_assert(sizeof(StoredG1Element) == 0x10);
} // namespace OpenRCT2
