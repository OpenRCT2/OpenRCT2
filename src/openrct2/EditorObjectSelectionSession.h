/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "core/EnumUtils.hpp"
#include "core/FlagHolder.hpp"
#include "localisation/StringIdType.h"
#include "object/ObjectTypes.h"

#include <optional>
#include <vector>

namespace OpenRCT2
{
    struct ObjectEntryDescriptor;
    struct ObjectRepositoryItem;
} // namespace OpenRCT2

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
extern uint32_t _numSelectedObjectsForType[EnumValue(OpenRCT2::ObjectType::count)];

bool EditorCheckObjectGroupAtLeastOneSelected(OpenRCT2::ObjectType checkObjectType);
bool EditorCheckObjectGroupAtLeastOneOfPeepTypeSelected(uint8_t peepType);
bool EditorCheckObjectGroupAtLeastOneSurfaceSelected(bool queue);
void EditorObjectFlagsClear();
void UnloadUnselectedObjects();
void Sub6AB211();
void ResetSelectedObjectCountAndSize();
void FinishObjectSelection();
ResultWithMessage WindowEditorObjectSelectionSelectObject(
    uint8_t isMasterObject, EditorInputFlags flags, const OpenRCT2::ObjectRepositoryItem* item);
ResultWithMessage WindowEditorObjectSelectionSelectObject(
    uint8_t isMasterObject, EditorInputFlags flags, const OpenRCT2::ObjectEntryDescriptor& entry);

/**
 * Removes all unused objects from the object selection.
 * @return The number of removed objects.
 */
int32_t EditorRemoveUnusedObjects();
