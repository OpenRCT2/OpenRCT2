/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../ride/RideTypes.h"
#include "../world/Location.hpp"

#include <memory>

struct Peep;
struct Guest;
struct TileElement;

// When the heuristic pathfinder is examining neighboring tiles, one possibility is that it finds a
// queue tile; furthermore, this queue tile may or may not be for the ride that the peep is trying
// to get to, if any. This first var is used to store the ride that the peep is currently headed to.
extern RideId gPeepPathFindQueueRideIndex;

// Furthermore, staff members don't care about this stuff; even if they are e.g. a mechanic headed
// to a particular ride, they have no issues with walking over queues for other rides to get there.
// This bool controls that behaviour - if true, the peep will not path over queues for rides other
// than their target ride, and if false, they will treat it like a regular path.
//
// In practice, if this is false, gPeepPathFindQueueRideIndex is always RIDE_ID_NULL.
extern bool gPeepPathFindIgnoreForeignQueues;

namespace OpenRCT2::PathFinding
{
    Direction ChooseDirection(const TileCoordsXYZ& loc, const TileCoordsXYZ& goal, Peep& peep);

    int32_t CalculateNextDestination(Guest& peep);

    int32_t GuestPathFindParkEntranceEntering(Peep& peep, uint8_t edges);

    int32_t GuestPathFindPeepSpawn(Peep& peep, uint8_t edges);

    int32_t GuestPathFindParkEntranceLeaving(Peep& peep, uint8_t edges);

    bool IsValidPathZAndDirection(TileElement* tileElement, int32_t currentZ, int32_t currentDirection);

}; // namespace OpenRCT2::PathFinding
