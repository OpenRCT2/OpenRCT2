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
namespace OpenRCT2
{
    struct ResultWithMessage;

    enum EDITOR_INPUT_FLAGS
    {
        INPUT_FLAG_EDITOR_OBJECT_SELECT = (1 << 0), // Set when you want to select an object, not set when you want to deselect
                                                    // it.
        INPUT_FLAG_EDITOR_OBJECT_1 = (1 << 1),
        INPUT_FLAG_EDITOR_OBJECT_SELECT_OBJECTS_IN_SCENERY_GROUP = (1 << 2),
        INPUT_FLAG_EDITOR_OBJECT_ALWAYS_REQUIRED = (1 << 3)
    };

    extern std::optional<StringId> _gSceneryGroupPartialSelectError;
    extern std::vector<uint8_t> _objectSelectionFlags;
    extern int32_t _numSelectedObjectsForType[EnumValue(ObjectType::Count)];

    bool EditorCheckObjectGroupAtLeastOneSelected(ObjectType checkObjectType);
    bool EditorCheckObjectGroupAtLeastOneSurfaceSelected(bool queue);
    void EditorObjectFlagsFree();
    void UnloadUnselectedObjects();
    void Sub6AB211();
    void ResetSelectedObjectCountAndSize();
    void FinishObjectSelection();
    ResultWithMessage WindowEditorObjectSelectionSelectObject(
        uint8_t isMasterObject, int32_t flags, const ObjectRepositoryItem* item);
    ResultWithMessage WindowEditorObjectSelectionSelectObject(
        uint8_t isMasterObject, int32_t flags, const ObjectEntryDescriptor& entry);

    /**
     * Removes all unused objects from the object selection.
     * @return The number of removed objects.
     */
    int32_t EditorRemoveUnusedObjects();
} // namespace OpenRCT2
