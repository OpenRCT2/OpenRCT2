/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../actions/CommandFlag.h"
#include "../actions/GameActionResult.h"
#include "Location.hpp"

#include <cstdint>

namespace OpenRCT2
{
    struct TileElement;
}

struct CoordsXY;
struct CoordsXYRangedZ;
class QuarterTile;

using ClearingFunction = bool (&)(
    OpenRCT2::TileElement** tile_element, const CoordsXY& coords, OpenRCT2::GameActions::CommandFlags flags, money64* price);

enum
{
    ELEMENT_IS_ABOVE_GROUND = 1 << 0,
    ELEMENT_IS_UNDERGROUND = 1 << 1,
    ELEMENT_IS_UNDERWATER = 1 << 2,
};

// Used when calling MapCanConstructWithClearAt();
// This assumes that the caller has already done the check on the element it wants to place,
// as this function can only check the element the player wants to build through.
enum class CreateCrossingMode
{
    none,
    trackOverPath,
    pathOverTrack,
};

bool MapPlaceNonSceneryClearFunc(
    OpenRCT2::TileElement** tile_element, const CoordsXY& coords, OpenRCT2::GameActions::CommandFlags flags, money64* price);
bool MapPlaceSceneryClearFunc(
    OpenRCT2::TileElement** tile_element, const CoordsXY& coords, OpenRCT2::GameActions::CommandFlags flags, money64* price);

struct ConstructClearResult
{
    uint8_t GroundFlags{ 0 };
};

[[nodiscard]] OpenRCT2::GameActions::Result MapCanConstructWithClearAt(
    const CoordsXYRangedZ& pos, ClearingFunction clearFunc, QuarterTile quarterTile, OpenRCT2::GameActions::CommandFlags flags,
    uint8_t slope, CreateCrossingMode crossingMode = CreateCrossingMode::none, bool isTree = false,
    RideId ignoreRideId = RideId::GetNull());

[[nodiscard]] OpenRCT2::GameActions::Result MapCanConstructAt(const CoordsXYRangedZ& pos, QuarterTile bl);

void MapGetObstructionErrorText(OpenRCT2::TileElement* tileElement, OpenRCT2::GameActions::Result& res);
