
#include "TextColour.h"

#include <array>

namespace OpenRCT2::Drawing
{
    static std::array<TextColours, kNumTextColours> kTextColourToPaletteIndex = { {
        {
            PaletteIndex::pi10,
            PaletteIndex::transparent,
            PaletteIndex::transparent,
        },
        {
            PaletteIndex::pi18,
            PaletteIndex::pi10,
            PaletteIndex::pi10,
        },
        {
            PaletteIndex::pi21,
            PaletteIndex::pi10,
            PaletteIndex::pi10,
        },
        {
            PaletteIndex::pi173,
            PaletteIndex::pi10,
            PaletteIndex::pi10,
        },
        {
            PaletteIndex::pi102,
            PaletteIndex::pi10,
            PaletteIndex::pi10,
        },
        {
            PaletteIndex::yellow9,
            PaletteIndex::pi10,
            PaletteIndex::pi10,
        },
        {
            PaletteIndex::pi187,
            PaletteIndex::pi10,
            PaletteIndex::pi10,
        },
        {
            PaletteIndex::pi152,
            PaletteIndex::pi10,
            PaletteIndex::pi10,
        },
        {
            PaletteIndex::pi139,
            PaletteIndex::pi10,
            PaletteIndex::pi10,
        },
        {
            PaletteIndex::pi164,
            PaletteIndex::pi10,
            PaletteIndex::pi10,
        },
        {
            PaletteIndex::pi43,
            PaletteIndex::pi10,
            PaletteIndex::pi10,
        },
        {
            PaletteIndex::pi69,
            PaletteIndex::pi10,
            PaletteIndex::pi10,
        },
        {
            PaletteIndex::pi199,
            PaletteIndex::pi10,
            PaletteIndex::pi10,
        },
        {
            PaletteIndex::pi223,
            PaletteIndex::pi10,
            PaletteIndex::pi10,
        },
    } };

    TextColours getTextColourMapping(TextColour textColour)
    {
        return kTextColourToPaletteIndex[static_cast<uint8_t>(textColour)];
    }
} // namespace OpenRCT2::Drawing
