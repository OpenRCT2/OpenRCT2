/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Cursors.h"

#include "../core/EnumMap.hpp"

#include <cassert>
#include <string>

namespace OpenRCT2::Cursor
{
    static const EnumMap<CursorID> LookupTable{
        { "CURSOR_BLANK", CursorID::blank },
        { "CURSOR_UP_ARROW", CursorID::upArrow },
        { "CURSOR_UP_DOWN_ARROW", CursorID::upDownArrow },
        { "CURSOR_HAND_POINT", CursorID::handPoint },
        { "CURSOR_ZZZ", CursorID::zzz },
        { "CURSOR_DIAGONAL_ARROWS", CursorID::diagonalArrows },
        { "CURSOR_PICKER", CursorID::picker },
        { "CURSOR_TREE_DOWN", CursorID::treeDown },
        { "CURSOR_FOUNTAIN_DOWN", CursorID::fountainDown },
        { "CURSOR_STATUE_DOWN", CursorID::statueDown },
        { "CURSOR_BENCH_DOWN", CursorID::benchDown },
        { "CURSOR_CROSS_HAIR", CursorID::crossHair },
        { "CURSOR_BIN_DOWN", CursorID::binDown },
        { "CURSOR_LAMPPOST_DOWN", CursorID::lamppostDown },
        { "CURSOR_FENCE_DOWN", CursorID::fenceDown },
        { "CURSOR_FLOWER_DOWN", CursorID::flowerDown },
        { "CURSOR_PATH_DOWN", CursorID::pathDown },
        { "CURSOR_DIG_DOWN", CursorID::digDown },
        { "CURSOR_WATER_DOWN", CursorID::waterDown },
        { "CURSOR_HOUSE_DOWN", CursorID::houseDown },
        { "CURSOR_VOLCANO_DOWN", CursorID::volcanoDown },
        { "CURSOR_WALK_DOWN", CursorID::walkDown },
        { "CURSOR_PAINT_DOWN", CursorID::paintDown },
        { "CURSOR_ENTRANCE_DOWN", CursorID::entranceDown },
        { "CURSOR_HAND_OPEN", CursorID::handOpen },
        { "CURSOR_HAND_CLOSED", CursorID::handClosed },
        { "CURSOR_ARROW", CursorID::arrow },
    };

    CursorID FromString(const std::string& s, CursorID defaultValue)
    {
        assert(defaultValue != CursorID::undefined);

        auto result = LookupTable.find(s);
        return (result != LookupTable.end()) ? result->second : defaultValue;
    }
} // namespace OpenRCT2::Cursor
