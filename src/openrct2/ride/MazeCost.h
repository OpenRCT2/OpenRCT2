/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Money.hpp"
#include "../world/Location.hpp"
#include "Ride.h"

namespace OpenRCT2
{
    money64 MazeCalculateCost(money64 constructionCost, const Ride& ride, const CoordsXYZ& loc);
}
