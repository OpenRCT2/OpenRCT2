/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../drawing/ImageIndexType.h"
#include "../Boundbox.h"

#include <cstdint>

struct Peep;
using Direction = uint8_t;
enum class PeepAnimationType : uint8_t;

constexpr BoundBoxXYZ kPaintPeepBoundBox(int32_t z)
{
    // In the calls to PaintAddImageAsParent/PaintAddImageAsChild, we add 5 (instead of 3) to the
    // offset.z to make sure peeps are drawn on top of railways.
    return BoundBoxXYZ{ { 0, 0, z + 5 }, { 1, 1, 11 } };
}
constexpr CoordsXYZ kPaintPeepOffset(int32_t z)
{
    return CoordsXYZ{ 0, 0, z };
}

struct BaseImageAndOffset
{
    ImageIndex baseImageId;
    uint8_t offset;
    PeepAnimationType actionAnimationGroup;
};

BaseImageAndOffset PaintPeepGetBaseImageAndOffset(const Peep& peep, Direction direction);
