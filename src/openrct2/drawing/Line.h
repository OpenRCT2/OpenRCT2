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

struct ScreenLine;

namespace OpenRCT2::Drawing
{
    enum class PaletteIndex : uint8_t;
    struct RenderTarget;

    void GfxDrawLine(RenderTarget& rt, const ScreenLine& line, PaletteIndex colour);
    void GfxDrawLineSoftware(RenderTarget& rt, const ScreenLine& line, PaletteIndex colour);
    void GfxDrawDashedLine(
        RenderTarget& rt, const ScreenLine& screenLine, int32_t dashedLineSegmentLength, PaletteIndex colour);
} // namespace OpenRCT2::Drawing
