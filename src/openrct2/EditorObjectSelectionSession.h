/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "common.h"
#include "object/Object.h"
#include "util/Util.h"

#include <vector>

struct ResultWithMessage;

enum EDITOR_INPUT_FLAGS
{
    INPUT_FLAG_EDITOR_OBJECT_SELECT = (1 << 0), // Set when you want to select an object, not set when you want to deselect it.
    INPUT_FLAG_EDITOR_OBJECT_1 = (1 << 1),
    INPUT_FLAG_EDITOR_OBJECT_SELECT_OBJECTS_IN_SCENERY_GROUP = (1 << 2),
    INPUT_FLAG_EDITOR_OBJECT_ALWAYS_REQUIRED = (1 << 3)
};

extern std::optional<StringId> _gSceneryGroupPartialSelectError;
extern std::vector<ObjectEntryDescriptor> _gErrorSceneryGroupObjectEntryDescriptors;
extern std::vector<uint8_t> _objectSelectionFlags;
extern int32_t _numSelectedObjectsForType[EnumValue(ObjectType::Count)];

bool editor_check_object_group_at_least_one_selected(ObjectType checkObjectType);
bool editor_check_object_group_at_least_one_surface_selected(bool queue);
void editor_object_flags_free();
void unload_unselected_objects();
void sub_6AB211();
void reset_selected_object_count_and_size();
void finish_object_selection();
ResultWithMessage window_editor_object_selection_select_object(
    uint8_t isMasterObject, int32_t flags, const ObjectRepositoryItem* item);
ResultWithMessage window_editor_object_selection_select_object(
    uint8_t isMasterObject, int32_t flags, const ObjectEntryDescriptor& entry);

/**
 * Removes all unused objects from the object selection.
 * @return The number of removed objects.
 */
int32_t editor_remove_unused_objects();
