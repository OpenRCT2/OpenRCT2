/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

struct CoordsXY;

enum class VirtualFloorStyles : int32_t
{
    Off,
    Clear,
    Glassy
};

struct PaintSession;

uint16_t VirtualFloorGetHeight();

bool VirtualFloorIsEnabled();
void VirtualFloorSetHeight(int16_t height);

void VirtualFloorEnable();
void VirtualFloorDisable();
void VirtualFloorInvalidate();

bool VirtualFloorTileIsFloor(const CoordsXY& loc);

void VirtualFloorPaint(PaintSession& session);
