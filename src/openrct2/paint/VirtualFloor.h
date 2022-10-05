/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

struct CoordsXY;

enum class VirtualFloorStyles : int32_t
{
    Off,
    Clear,
    Glassy
};

struct paint_session;

uint16_t virtual_floor_get_height();

bool virtual_floor_is_enabled();
void virtual_floor_set_height(int16_t height);

void virtual_floor_enable();
void virtual_floor_disable();
void virtual_floor_invalidate();

bool virtual_floor_tile_is_floor(const CoordsXY& loc);

void virtual_floor_paint(paint_session& session);
