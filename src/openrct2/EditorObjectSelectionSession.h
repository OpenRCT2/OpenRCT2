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

#include <vector>
#include "common.h"
#include "object/Object.h"

extern bool _maxObjectsWasHit;
extern std::vector<uint8> _objectSelectionFlags;
extern sint32 _numSelectedObjectsForType[OBJECT_TYPE_COUNT];

bool editor_check_object_group_at_least_one_selected(sint32 objectType);
void editor_object_flags_free();
void unload_unselected_objects();
void sub_6AB211();
void reset_selected_object_count_and_size();
sint32 window_editor_object_selection_select_object(uint8 bh, sint32 flags, const rct_object_entry *entry);

/**
* Removes all unused objects from the object selection.
* @return The number of removed objects.
*/
sint32 editor_remove_unused_objects();

