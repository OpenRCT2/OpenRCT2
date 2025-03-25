/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "core/EnumUtils.hpp"
#include "core/FlagHolder.hpp"
#include "object/Object.h"

#include <vector>

struct ResultWithMessage;

enum class EditorInputFlag : uint8_t
{
    select, // Set when you want to select an object, not set when you want to deselect it.
    unk1,
    selectObjectsInSceneryGroup,
    objectAlwaysRequired,
};
using EditorInputFlags = FlagHolder<uint8_t, EditorInputFlag>;

extern std::optional<StringId> _gSceneryGroupPartialSelectError;
extern std::vector<uint8_t> _objectSelectionFlags;
extern uint32_t _numSelectedObjectsForType[EnumValue(ObjectType::count)];

bool EditorCheckObjectGroupAtLeastOneSelected(ObjectType checkObjectType);
bool EditorCheckObjectGroupAtLeastOneOfPeepTypeSelected(uint8_t peepType);
bool EditorCheckObjectGroupAtLeastOneSurfaceSelected(bool queue);
void EditorObjectFlagsFree();
void UnloadUnselectedObjects();
void Sub6AB211();
void ResetSelectedObjectCountAndSize();
void FinishObjectSelection();
ResultWithMessage WindowEditorObjectSelectionSelectObject(
    uint8_t isMasterObject, EditorInputFlags flags, const ObjectRepositoryItem* item);
ResultWithMessage WindowEditorObjectSelectionSelectObject(
    uint8_t isMasterObject, EditorInputFlags flags, const ObjectEntryDescriptor& entry);

/**
 * Removes all unused objects from the object selection.
 * @return The number of removed objects.
 */
int32_t EditorRemoveUnusedObjects();
