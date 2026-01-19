/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RenderTarget.h"

namespace OpenRCT2::Drawing
{
    uint8_t* RenderTarget::GetBitsOffset(const ScreenCoordsXY& pos) const
    {
        return bits + pos.x + pos.y * LineStride();
    }

    RenderTarget RenderTarget::Crop(const ScreenCoordsXY& pos, const ScreenSize& size) const
    {
        RenderTarget result = *this;
        result.bits = GetBitsOffset(pos);
        result.x = pos.x;
        result.y = pos.y;
        result.width = size.width;
        result.height = size.height;
        result.pitch = width + pitch - size.width;
        return result;
    }
} // namespace OpenRCT2::Drawing
