#pragma once

#include "../ride/Ride.h"
#include "../world/Banner.h"
#include "../world/Entrance.h"
#include "../world/Footpath.h"
#include "../world/Scenery.h"
#include "../world/Water.h"
#include "ObjectLimits.h"

void get_type_entry_index(size_t index, uint8 * outObjectType, uint8 * outEntryIndex);
const rct_object_entry * get_loaded_object_entry(size_t index);
void * get_loaded_object_chunk(size_t index);
uint8 object_entry_get_type(const rct_object_entry * objectEntry);
uint8 object_entry_get_source_game(const rct_object_entry * objectEntry);
