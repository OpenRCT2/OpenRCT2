/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _MAZE_H_
#define _MAZE_H_

#include "../../peep/Peep.h"
#include "../../world/Location.hpp"
#include "../Ride.h"
#include "../RideTypes.h"

#include <map>
#include <memory>
#include <set>
#include <utility>

/**
 * Get the new maze subTileIndex as used by Peep::var_37 from the previous subTitleIndex and a direction.
 * Div the returned value by 4 to get the subTileIndex as used the MazeDataPath methods.
 */
extern const uint8_t gMazeGetNewDirectionFromEdge[][4];

/**
 * Get the bit index of the flag in a mazeEntry indicating the presence of a hedge for a subTileIndex and a direction.
 */
extern const uint8_t gMazeCurrentDirectionToHedge[][4];

#endif
