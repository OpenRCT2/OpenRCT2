/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
} // namespace OpenRCT2::PaintSessionFlags
