/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Cursors.h"

#include "../core/EnumMap.hpp"

#include <string>

namespace Cursor
{
    static const EnumMap<CursorID> LookupTable{
        { "CURSOR_BLANK", CursorID::Blank },
        { "CURSOR_UP_ARROW", CursorID::UpArrow },
        { "CURSOR_UP_DOWN_ARROW", CursorID::UpDownArrow },
        { "CURSOR_HAND_POINT", CursorID::HandPoint },
        { "CURSOR_ZZZ", CursorID::ZZZ },
        { "CURSOR_DIAGONAL_ARROWS", CursorID::DiagonalArrows },
        { "CURSOR_PICKER", CursorID::Picker },
        { "CURSOR_TREE_DOWN", CursorID::TreeDown },
        { "CURSOR_FOUNTAIN_DOWN", CursorID::FountainDown },
        { "CURSOR_STATUE_DOWN", CursorID::StatueDown },
        { "CURSOR_BENCH_DOWN", CursorID::BenchDown },
        { "CURSOR_CROSS_HAIR", CursorID::CrossHair },
        { "CURSOR_BIN_DOWN", CursorID::BinDown },
        { "CURSOR_LAMPPOST_DOWN", CursorID::LamppostDown },
        { "CURSOR_FENCE_DOWN", CursorID::FenceDown },
        { "CURSOR_FLOWER_DOWN", CursorID::FlowerDown },
        { "CURSOR_PATH_DOWN", CursorID::PathDown },
        { "CURSOR_DIG_DOWN", CursorID::DigDown },
        { "CURSOR_WATER_DOWN", CursorID::WaterDown },
        { "CURSOR_HOUSE_DOWN", CursorID::HouseDown },
        { "CURSOR_VOLCANO_DOWN", CursorID::VolcanoDown },
        { "CURSOR_WALK_DOWN", CursorID::WalkDown },
        { "CURSOR_PAINT_DOWN", CursorID::PaintDown },
        { "CURSOR_ENTRANCE_DOWN", CursorID::EntranceDown },
        { "CURSOR_HAND_OPEN", CursorID::HandOpen },
        { "CURSOR_HAND_CLOSED", CursorID::HandClosed },
        { "CURSOR_ARROW", CursorID::Arrow },
    };

    CursorID FromString(std::string_view s, CursorID defaultValue)
    {
        assert(defaultValue != CursorID::Undefined);

        auto result = LookupTable.find(s);
        return (result != LookupTable.end()) ? result->second : defaultValue;
    }
} // namespace Cursor
