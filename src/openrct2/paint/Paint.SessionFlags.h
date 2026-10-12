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

namespace OpenRCT2::PaintSessionFlags
{
    constexpr uint8_t PassedSurface = 1u << 0;
    constexpr uint8_t IsTrackPiecePreview = 1u << 1;
    /** Set while painting the vertical faces of a terrain or water tile, so the paint structs
        they produce can be told apart from the flat top. Nothing about the bound box says
        which is which: a raised path deck has the same shape as a cliff face. */
    constexpr uint8_t PaintingVerticalTileSide = 1u << 2;
    constexpr uint8_t PaintingTerrainCap = 1u << 3;
} // namespace OpenRCT2::PaintSessionFlags
