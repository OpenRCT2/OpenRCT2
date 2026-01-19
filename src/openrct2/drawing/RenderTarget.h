/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../interface/ZoomLevel.h"
#include "../world/Location.hpp"

namespace OpenRCT2::Drawing
{
    struct IDrawingEngine;

    struct RenderTarget
    {
        uint8_t* bits{};
        int32_t x{};
        int32_t y{};
        int32_t width{};
        int32_t height{};
        int32_t pitch{}; // note: this is actually (pitch - width)
        int32_t cullingX{};
        int32_t cullingY{};
        int32_t cullingWidth{};
        int32_t cullingHeight{};
        ZoomLevel zoom_level{};

        // Last position of drawn text.
        ScreenCoordsXY lastStringPos{};

        IDrawingEngine* DrawingEngine{};

        uint8_t* GetBitsOffset(const ScreenCoordsXY& pos) const;
        RenderTarget Crop(const ScreenCoordsXY& pos, const ScreenSize& size) const;

        [[nodiscard]] constexpr int32_t WorldX() const
        {
            return zoom_level.ApplyTo(x);
        }
        [[nodiscard]] constexpr int32_t WorldY() const
        {
            return zoom_level.ApplyTo(y);
        }
        [[nodiscard]] constexpr int32_t WorldWidth() const
        {
            return zoom_level.ApplyTo(width);
        }
        [[nodiscard]] constexpr int32_t WorldHeight() const
        {
            return zoom_level.ApplyTo(height);
        }

        [[nodiscard]] constexpr int32_t LineStride() const
        {
            return width + pitch;
        }
    };
} // namespace OpenRCT2::Drawing
