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

#include "../common.h"
#include "map.h"

#ifdef __cplusplus
extern "C" {
#endif

sint32 scenery_small_get_primary_colour(const rct_tile_element * tileElement);
sint32 scenery_small_get_secondary_colour(const rct_tile_element * tileElement);
void scenery_small_set_primary_colour(rct_tile_element * tileElement, uint32 colour);
void scenery_small_set_secondary_colour(rct_tile_element * tileElement, uint32 colour);
bool scenery_small_get_supports_needed(const rct_tile_element * tileElement);
void scenery_small_set_supports_needed(rct_tile_element * tileElement);

#ifdef __cplusplus
}
#endif
