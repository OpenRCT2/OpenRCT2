/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <vector>
#include "common.h"
#include "object/Object.h"

extern bool _maxObjectsWasHit;
extern std::vector<uint8_t> _objectSelectionFlags;
extern int32_t _numSelectedObjectsForType[OBJECT_TYPE_COUNT];

bool editor_check_object_group_at_least_one_selected(int32_t objectType);
void editor_object_flags_free();
void unload_unselected_objects();
void sub_6AB211();
void reset_selected_object_count_and_size();
int32_t window_editor_object_selection_select_object(uint8_t bh, int32_t flags, const rct_object_entry *entry);

/**
* Removes all unused objects from the object selection.
* @return The number of removed objects.
*/
int32_t editor_remove_unused_objects();

