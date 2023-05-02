/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

#include <string>

enum class CursorID : uint8_t
{
    Arrow,
    Blank,
    UpArrow,
    UpDownArrow,
    HandPoint,
    ZZZ,
    DiagonalArrows,
    Picker,
    TreeDown,
    FountainDown,
    StatueDown,
    BenchDown,
    CrossHair,
    BinDown,
    LamppostDown,
    FenceDown,
    FlowerDown,
    PathDown,
    DigDown,
    WaterDown,
    HouseDown,
    VolcanoDown,
    WalkDown,
    PaintDown,
    EntranceDown,
    HandOpen,
    HandClosed,
    Count,

    Undefined = 0xFF
};

namespace Cursor
{
    CursorID FromString(const std::string& s, CursorID defaultValue);
}

namespace OpenRCT2::Ui
{
    constexpr size_t CURSOR_BIT_WIDTH = 32;
    constexpr size_t CURSOR_HEIGHT = 4;
    struct CursorData
    {
        struct HotSpot
        {
            int16_t X;
            int16_t Y;
        } HotSpot;
        uint8_t Data[CURSOR_BIT_WIDTH * CURSOR_HEIGHT];
        uint8_t Mask[CURSOR_BIT_WIDTH * CURSOR_HEIGHT];
    };
} // namespace OpenRCT2::Ui
