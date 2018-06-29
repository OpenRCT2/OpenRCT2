/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "Map.h"
#include "TileElement.h"

colour_t scenery_large_get_primary_colour(const rct_tile_element * tileElement);
colour_t scenery_large_get_secondary_colour(const rct_tile_element * tileElement);
void scenery_large_set_primary_colour(rct_tile_element * tileElement, colour_t colour);
void scenery_large_set_secondary_colour(rct_tile_element * tileElement, colour_t colour);
BannerIndex scenery_large_get_banner_id(const rct_tile_element* tileElement);
void scenery_large_set_banner_id(rct_tile_element * tileElement, BannerIndex bannerIndex);
int32_t scenery_large_get_type(const rct_tile_element * tileElement);
int32_t scenery_large_get_sequence(const rct_tile_element * tileElement);
void scenery_large_set_type(rct_tile_element * tileElement, uint16_t type);
void scenery_large_set_sequence(rct_tile_element * tileElement, uint16_t sequence);
