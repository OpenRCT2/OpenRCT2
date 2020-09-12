/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _GUESTPATHFINDING_H_
#define _GUESTPATHFINDING_H_

#include "../common.h"
#include "../ride/RideTypes.h"
#include "../world/Location.hpp"

struct Peep;
struct Guest;
struct TileElement;

extern TileCoordsXYZ gPeepPathFindGoalPosition;
extern bool gPeepPathFindIgnoreForeignQueues;
extern ride_id_t gPeepPathFindQueueRideIndex;

Direction peep_pathfind_choose_direction(const TileCoordsXYZ& loc, Peep* peep);
void peep_reset_pathfind_goal(Peep* peep);

bool is_valid_path_z_and_direction(TileElement* tileElement, int32_t currentZ, int32_t currentDirection);

int32_t guest_path_finding(Guest* peep);

#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
#    define PATHFIND_DEBUG                                                                                                     \
        0 // Set to 0 to disable pathfinding debugging;
          // Set to 1 to enable pathfinding debugging.

// When PATHFIND_DEBUG is 1 (nonzero):
// The following calls configure debug logging for the given peep
// If they're a guest, pathfinding will be logged if they have PEEP_FLAGS_TRACKING set
// If they're staff, pathfinding will be logged if their name is "Mechanic Debug"
void pathfind_logging_enable(Peep* peep);
void pathfind_logging_disable();
#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

#endif
