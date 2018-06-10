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
#include "Map.h"
#include "TileElement.h"

colour_t scenery_large_get_primary_colour(const rct_tile_element * tileElement);
colour_t scenery_large_get_secondary_colour(const rct_tile_element * tileElement);
void scenery_large_set_primary_colour(rct_tile_element * tileElement, colour_t colour);
void scenery_large_set_secondary_colour(rct_tile_element * tileElement, colour_t colour);
sint32 scenery_large_get_banner_id(const rct_tile_element * tileElement);
void scenery_large_set_banner_id(rct_tile_element * tileElement, uint8 bannerIndex);
sint32 scenery_large_get_type(const rct_tile_element * tileElement);
sint32 scenery_large_get_sequence(const rct_tile_element * tileElement);
void scenery_large_set_type(rct_tile_element * tileElement, uint16 type);
void scenery_large_set_sequence(rct_tile_element * tileElement, uint16 sequence);
