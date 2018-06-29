/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../ride/Ride.h"
#include "../world/Banner.h"
#include "../world/Entrance.h"
#include "../world/Footpath.h"
#include "../world/Scenery.h"
#include "../world/Water.h"
#include "ObjectLimits.h"

void get_type_entry_index(size_t index, uint8_t * outObjectType, uint8_t * outEntryIndex);
const rct_object_entry * get_loaded_object_entry(size_t index);
void * get_loaded_object_chunk(size_t index);
uint8_t object_entry_get_type(const rct_object_entry * objectEntry);
uint8_t object_entry_get_source_game(const rct_object_entry * objectEntry);
