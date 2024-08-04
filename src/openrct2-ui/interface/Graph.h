/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/core/Money.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/world/Location.hpp>

namespace OpenRCT2::Graph
{
    void Draw(DrawPixelInfo& dpi, uint8_t* history, int32_t count, const ScreenCoordsXY& screenPos);
    void Draw(
        DrawPixelInfo& dpi, const money64* history, const int32_t count, const ScreenCoordsXY& coords, const int32_t modifier,
        const int32_t offset);
} // namespace OpenRCT2::Graph
