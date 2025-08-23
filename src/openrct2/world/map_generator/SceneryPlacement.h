/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Location.hpp"
#include "rule/Rule.h"

namespace OpenRCT2::World::MapGenerator
{
    void placeScenery(const TileCoordsXY& loc, const Rule::SceneryResult& sceneryResult);
} // namespace OpenRCT2::World::MapGenerator
