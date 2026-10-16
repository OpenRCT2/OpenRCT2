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

using Direction = uint8_t;
struct TileCornersZ;

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

    constexpr QuarterTile Rotate(const Direction amount) const
    {
        switch (amount)
        {
            case 0:
                return QuarterTile(_val);
            case 1:
            {
                auto rotVal1 = _val << 1;
                auto rotVal2 = rotVal1 >> 4;
                // Clear the bit from the tileQuarter
                rotVal1 &= 0b11101110;
                // Clear the bit from the zQuarter
                rotVal2 &= 0b00010001;
                return QuarterTile(static_cast<uint8_t>(rotVal1 | rotVal2));
            }
            case 2:
            {
                auto rotVal1 = _val << 2;
                auto rotVal2 = rotVal1 >> 4;
                // Clear the bit from the tileQuarter
                rotVal1 &= 0b11001100;
                // Clear the bit from the zQuarter
                rotVal2 &= 0b00110011;
                return QuarterTile(static_cast<uint8_t>(rotVal1 | rotVal2));
            }
            case 3:
            {
                auto rotVal1 = _val << 3;
                auto rotVal2 = rotVal1 >> 4;
                // Clear the bit from the tileQuarter
                rotVal1 &= 0b10001000;
                // Clear the bit from the zQuarter
                rotVal2 &= 0b01110111;
                return QuarterTile(static_cast<uint8_t>(rotVal1 | rotVal2));
            }
            default:
                return QuarterTile(0);
        }
    }

    constexpr uint8_t GetBaseQuarterOccupied() const
    {
        return _val & 0xF;
    }

    constexpr uint8_t GetZQuarterOccupied() const
    {
        return (_val >> 4) & 0xF;
    }

    TileCornersZ GetQuarterHeights(int32_t height) const;
};

enum
{
    TILE_ELEMENT_QUADRANT_SW,
    TILE_ELEMENT_QUADRANT_NW,
    TILE_ELEMENT_QUADRANT_NE,
    TILE_ELEMENT_QUADRANT_SE
};
