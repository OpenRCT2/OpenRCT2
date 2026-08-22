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

using ImageIndex = uint32_t;

struct ImageId;
struct ScreenCoordsXY;
struct ZoomLevel;

namespace OpenRCT2::Drawing
{
    enum class Colour : uint8_t;
    struct RenderTarget;

    void pickupPeepSetImage(ImageIndex baseImageId, Colour primaryColour, Colour secondaryColour);
    void pickupPeepSetPosition(ScreenCoordsXY position);
    void pickupPeepClear();
    void pickupPeepInvalidate();
    void pickupPeepDraw(RenderTarget& rt);
} // namespace OpenRCT2::Drawing
