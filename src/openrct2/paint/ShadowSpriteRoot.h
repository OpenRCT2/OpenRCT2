/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Location.hpp"

#include <optional>

struct PaintStruct;

namespace OpenRCT2
{
    struct EntityBase;
    struct G1Element;
} // namespace OpenRCT2

/**
 * Where a peep's or prop's shadow card stands, and how far the shadow pass must shift its sprite.
 *
 * The camera and the sun each draw their own direction's image of an object, and where the shadow
 * lands on the ground depends only on where the sun draws its image. So both views find the same
 * world point - the root - in their own image, and the sun's image is drawn shifted to put its
 * root on the camera's. Outside the shadow pass the offset is zero.
 */
struct ShadowRoot
{
    CoordsXY world;
    ScreenCoordsXY drawOffset;
};

/** A peep stands a constant foot drop below its entity position: rooting on the lowest pixel made
    the shadow follow whichever foot was down. */
ShadowRoot PeepShadowRoot(const OpenRCT2::EntityBase& entity, uint8_t rotation);

/** A prop stands on the centre of the footprint its artwork shows, projected onto baseZ. Nothing
    if the sprite has no opaque texels. */
std::optional<ShadowRoot> PropShadowRoot(const PaintStruct& ps, const OpenRCT2::G1Element& g1, int32_t baseZ, uint8_t rotation);
