#pragma region Copyright (c) 2014-2018 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef _VIRTUAL_FLOOR_H
#define _VIRTUAL_FLOOR_H

#include "../common.h"

struct paint_session;

uint16 virtual_floor_get_height();

bool virtual_floor_is_enabled();
void virtual_floor_set_height(sint16 height);

void virtual_floor_enable();
void virtual_floor_disable();
void virtual_floor_invalidate();

bool virtual_floor_tile_is_floor(sint16 x, sint16 y);

void virtual_floor_paint(paint_session * session);

#endif
