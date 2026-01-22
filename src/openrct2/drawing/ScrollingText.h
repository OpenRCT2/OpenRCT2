/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

struct ImageId;
struct PaintSession;
using StringId = uint16_t;

constexpr uint8_t kScrollingModeNone = 255;

namespace OpenRCT2
{
    class Formatter;
}

namespace OpenRCT2::Drawing
{
    enum class PaletteIndex : uint8_t;
}

namespace OpenRCT2::Drawing::ScrollingText
{
    static auto constexpr kMaxLegacyEntries = 32;
    static auto constexpr kMaxEntries = 256;
    constexpr int8_t kMaxModes = 38;

    void initialiseBitmaps();
    void invalidate();
    ImageId setup(
        PaintSession& session, StringId stringId, Formatter& ft, uint16_t scroll, uint16_t scrollingMode, PaletteIndex colour);
} // namespace OpenRCT2::Drawing::ScrollingText
