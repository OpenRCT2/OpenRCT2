/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../core/EnumUtils.hpp"
#include "../../core/FlagHolder.hpp"
#include "../../core/StringTypes.h"
#include "../../localisation/StringIdType.h"
#include "../../object/ObjectTypes.h"

#include <vector>

struct ResultWithMessage;
struct ResultWithMessageString;

namespace OpenRCT2
{
    struct ObjectEntryDescriptor;
    struct ObjectRepositoryItem;
    enum class ObjectSelectionFlag : uint8_t;
    using ObjectSelectionFlags = FlagHolder<uint8_t, ObjectSelectionFlag>;
} // namespace OpenRCT2

namespace OpenRCT2::Editor
{
    enum class InputFlag : uint8_t
    {
        select, // Set when you want to select an object, not set when you want to deselect it.
        unk1,
        selectObjectsInSceneryGroup,
        objectAlwaysRequired,
    };
    using InputFlags = FlagHolder<uint8_t, InputFlag>;

    extern u8string gSceneryGroupPartialSelectError;
    extern std::vector<ObjectSelectionFlags> _objectSelectionFlags;
    extern uint32_t _numSelectedObjectsForType[EnumValue(ObjectType::count)];

    void ObjectListLoad();
    ResultWithMessage CheckPark();
    std::pair<ObjectType, StringId> CheckObjectSelection();

    ObjectSelectionFlags GetSelectedObjectFlags(ObjectType objectType, size_t index);
    void ClearSelectedObject(ObjectType objectType, size_t index);
    void SetSelectedObject(ObjectType objectType, size_t index, ObjectSelectionFlags flags);

    bool CheckObjectGroupAtLeastOneSelected(ObjectType checkObjectType);
    bool CheckObjectGroupAtLeastOneOfPeepTypeSelected(uint8_t peepType);
    bool CheckObjectGroupAtLeastOneSurfaceSelected(bool queue);
    void ObjectFlagsClear();
    void UnloadUnselectedObjects();
    void Sub6AB211();
    void ResetSelectedObjectCountAndSize();
    void FinishObjectSelection();

    ResultWithMessageString ObjectSelectionSelectObject(
        uint8_t isMasterObject, InputFlags flags, const ObjectRepositoryItem* item);
    ResultWithMessageString ObjectSelectionSelectObject(
        uint8_t isMasterObject, InputFlags flags, const ObjectEntryDescriptor& descriptor);

    /**
     * Removes all unused objects from the object selection.
     * @return The number of removed objects.
     */
    int32_t RemoveUnusedObjects();
} // namespace OpenRCT2::Editor
