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

class QuarterTile
{
private:
    uint8_t _val{ 0 };

public:
    constexpr QuarterTile(uint8_t tileQuarter, uint8_t zQuarter)
        : _val(tileQuarter | (zQuarter << 4))
    {
    }

    constexpr QuarterTile(uint8_t tileAndZQuarter)
        : _val(tileAndZQuarter)
    {
    }

    // Rotate both of the values amount. Returns new RValue QuarterTile
    const QuarterTile Rotate(uint8_t amount) const;
    uint8_t GetBaseQuarterOccupied() const;
    uint8_t GetZQuarterOccupied() const;
};

enum
{
    TILE_ELEMENT_QUADRANT_SW,
    TILE_ELEMENT_QUADRANT_NW,
    TILE_ELEMENT_QUADRANT_NE,
    TILE_ELEMENT_QUADRANT_SE
};
