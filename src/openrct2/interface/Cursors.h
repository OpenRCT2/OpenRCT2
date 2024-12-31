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
    Bulldozer,
    Count,

    Undefined = 0xFF
};

namespace OpenRCT2::Cursor
{
    CursorID FromString(const std::string& s, CursorID defaultValue);
}
