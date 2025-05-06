/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Location.hpp"

#include <cstdint>

struct TileElementBase;

void MapAnimationCreate(const CoordsXY& coords, const TileElementBase* tileElement);
void MapAnimationCreate(const CoordsXY& coords);
void MapAnimationUpdateAll();
void ClearMapAnimations();
void MapAnimationAutoCreate();
void ShiftAllMapAnimations(CoordsXY amount);
