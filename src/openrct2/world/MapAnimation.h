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

namespace OpenRCT2::MapAnimation
{
    void Create(const CoordsXY coords);
    void CreateAll();
    void UpdateAll();
    void ClearAll();
    void ShiftAll(const CoordsXY amount);
} // namespace OpenRCT2::MapAnimation
