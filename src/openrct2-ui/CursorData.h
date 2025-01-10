/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/interface/Cursors.h>

namespace OpenRCT2::Ui
{
    constexpr size_t kCursorBitWidth = 32;
    constexpr size_t kCursorHeight = 4;
    constexpr size_t kRawCursorSize = kCursorBitWidth * kCursorBitWidth;
    constexpr size_t kEncodedCursorSize = kRawCursorSize / 8;

    struct CursorData
    {
        struct HotSpot
        {
            int16_t X;
            int16_t Y;
        } HotSpot;
        uint8_t Data[kEncodedCursorSize];
        uint8_t Mask[kEncodedCursorSize];
    };

    const CursorData* getCursorData(CursorID cursorId);
} // namespace OpenRCT2::Ui
