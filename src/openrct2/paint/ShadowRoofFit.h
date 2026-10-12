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
#include <optional>

struct PaintStruct;

namespace OpenRCT2
{
    struct G1Element;
}

/**
 * The roof solid a roof prop's sprite shows, as a shape code: shape | rise direction << 4 |
 * height << 8, matching roofHeight() in drawrect.frag. Nothing for a prop that is not a roof or
 * that fits no candidate solid well. Fitted once per object and placement, then cached.
 */
std::optional<int32_t> RoofShapeFor(const PaintStruct& ps, const OpenRCT2::G1Element& g1, uint8_t rotation);
