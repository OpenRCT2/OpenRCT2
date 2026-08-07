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
#include <string>

enum class CursorID : uint8_t
{
    arrow,
    blank,
    upArrow,
    upDownArrow,
    handPoint,
    zzz,
    diagonalArrows,
    picker,
    treeDown,
    fountainDown,
    statueDown,
    benchDown,
    crossHair,
    binDown,
    lamppostDown,
    fenceDown,
    flowerDown,
    pathDown,
    digDown,
    waterDown,
    houseDown,
    volcanoDown,
    walkDown,
    paintDown,
    entranceDown,
    handOpen,
    handClosed,
    bulldozer,
    count,

    undefined = 0xFF
};

namespace OpenRCT2::Cursor
{
    CursorID FromString(const std::string& s, CursorID defaultValue);
}
