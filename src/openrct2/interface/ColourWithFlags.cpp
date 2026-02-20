/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ColourWithFlags.h"

namespace OpenRCT2
{
    static constexpr uint8_t kLegacyColourMaskBase = 0x1F;
    static constexpr uint8_t kLegacyColourFlagOutline = (1 << 5);
    static constexpr uint8_t kLegacyColourFlagInset = (1 << 6);

    ColourWithFlags ColourWithFlags::withFlag(ColourFlag flag, bool on) const
    {
        struct ColourWithFlags result = *this;
        result.flags.set(flag, on);
        return result;
    }

    ColourWithFlags ColourWithFlags::fromLegacy(uint8_t legacy)
    {
        ColourWithFlags result{};
        result.colour = static_cast<Drawing::Colour>(legacy & kLegacyColourMaskBase);
        if (legacy & kLegacyColourFlagTranslucent)
            result.flags.set(ColourFlag::translucent);
        if (legacy & kLegacyColourFlagInset)
            result.flags.set(ColourFlag::inset);
        if (legacy & kLegacyColourFlagOutline)
            result.flags.set(ColourFlag::withOutline);

        return result;
    }

    ColourWithFlags& ColourWithFlags::operator=(Drawing::Colour rhs)
    {
        colour = rhs;
        flags = {};
        return *this;
    }
} // namespace OpenRCT2
