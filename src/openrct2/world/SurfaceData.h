/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <cstdio>

uint8_t LowerSurfaceCornerFlags(size_t SelectedCorner, size_t CurrentSlope);
uint8_t RaiseSurfaceCornerFlags(size_t SelectedCorner, size_t CurrentSlope);
