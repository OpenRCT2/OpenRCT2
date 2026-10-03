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

constexpr auto kNumOrthogonalDirections = 4;

/**
 * Cardinal directions are represented by the Direction type. It has four
 * possible values:
 * 0 is X-decreasing (West)
 * 1 is Y-increasing (North)
 * 2 is X-increasing (East)
 * 3 is Y-decreasing (South)
 * Direction is not used to model up/down, or diagonal directions.
 */
using Direction = uint8_t;

const Direction kInvalidDirection = 0xFF;

/**
 * Array of all valid cardinal directions, to make it easy to write range-based for loops like:
 *   for (Direction d : kAllDirections)
 */
constexpr Direction kAllDirections[] = {
    0,
    1,
    2,
    3,
};

/**
 * Given a direction, return the direction that points the other way,
 * on the same axis.
 */
inline constexpr Direction DirectionReverse(Direction dir)
{
    return dir ^ 2;
}

inline constexpr bool DirectionValid(Direction dir)
{
    return dir < kNumOrthogonalDirections;
}

/**
 * Given a direction, return the next clockwise cardinal direction, wrapping around if necessary.
 */
inline constexpr Direction DirectionNext(Direction dir)
{
    return (dir + 1) & 0x03;
}

/**
 * Given a direction, return the next counter-clockwise cardinal direction, wrapping around if necessary.
 */
inline constexpr Direction DirectionPrev(Direction dir)
{
    return (dir - 1) & 0x03;
}

/*
 * Flips the X axis so 1 and 3 are swapped 0 and 2 will stay the same.
 */
inline constexpr Direction DirectionFlipXAxis(Direction direction)
{
    return (direction * 3) % 4;
}

