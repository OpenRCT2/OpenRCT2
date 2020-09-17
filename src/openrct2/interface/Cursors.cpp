/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Cursors.h"

#include <string>
#include <unordered_map>

namespace Cursor
{
    uint8_t FromString(const std::string& s, uint8_t defaultValue)
    {
        static const std::unordered_map<std::string, uint8_t> LookupTable{
            { "CURSOR_BLANK", CURSOR_BLANK },
            { "CURSOR_UP_ARROW", CURSOR_UP_ARROW },
            { "CURSOR_UP_DOWN_ARROW", CURSOR_UP_DOWN_ARROW },
            { "CURSOR_HAND_POINT", CURSOR_HAND_POINT },
            { "CURSOR_ZZZ", CURSOR_ZZZ },
            { "CURSOR_DIAGONAL_ARROWS", CURSOR_DIAGONAL_ARROWS },
            { "CURSOR_PICKER", CURSOR_PICKER },
            { "CURSOR_TREE_DOWN", CURSOR_TREE_DOWN },
            { "CURSOR_FOUNTAIN_DOWN", CURSOR_FOUNTAIN_DOWN },
            { "CURSOR_STATUE_DOWN", CURSOR_STATUE_DOWN },
            { "CURSOR_BENCH_DOWN", CURSOR_BENCH_DOWN },
            { "CURSOR_CROSS_HAIR", CURSOR_CROSS_HAIR },
            { "CURSOR_BIN_DOWN", CURSOR_BIN_DOWN },
            { "CURSOR_LAMPPOST_DOWN", CURSOR_LAMPPOST_DOWN },
            { "CURSOR_FENCE_DOWN", CURSOR_FENCE_DOWN },
            { "CURSOR_FLOWER_DOWN", CURSOR_FLOWER_DOWN },
            { "CURSOR_PATH_DOWN", CURSOR_PATH_DOWN },
            { "CURSOR_DIG_DOWN", CURSOR_DIG_DOWN },
            { "CURSOR_WATER_DOWN", CURSOR_WATER_DOWN },
            { "CURSOR_HOUSE_DOWN", CURSOR_HOUSE_DOWN },
            { "CURSOR_VOLCANO_DOWN", CURSOR_VOLCANO_DOWN },
            { "CURSOR_WALK_DOWN", CURSOR_WALK_DOWN },
            { "CURSOR_PAINT_DOWN", CURSOR_PAINT_DOWN },
            { "CURSOR_ENTRANCE_DOWN", CURSOR_ENTRANCE_DOWN },
            { "CURSOR_HAND_OPEN", CURSOR_HAND_OPEN },
            { "CURSOR_HAND_CLOSED", CURSOR_HAND_CLOSED },
            { "CURSOR_ARROW", CURSOR_ARROW },
        };

        auto result = LookupTable.find(s);
        return (result != LookupTable.end()) ? result->second : defaultValue;
    }
} // namespace Cursor
