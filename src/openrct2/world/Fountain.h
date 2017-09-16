#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#pragma once

#include "../common.h"

#include "map.h"
#include "sprite.h"

enum
{
    JUMPING_FOUNTAIN_TYPE_WATER,
    JUMPING_FOUNTAIN_TYPE_SNOW
};

#ifdef __cplusplus
extern "C"
{
#endif
    void jumping_fountain_begin(sint32 type, sint32 x, sint32 y, const rct_map_element * mapElement);
    void jumping_fountain_create(sint32 type, sint32 x, sint32 y, sint32 z, sint32 direction, sint32 flags, sint32 iteration);
    void jumping_fountain_update(rct_jumping_fountain * jumpingFountain);
#ifdef __cplusplus
}
#endif
