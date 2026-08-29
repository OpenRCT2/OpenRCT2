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

    enum class Step : uint8_t;

    extern u8string gSceneryGroupPartialSelectError;
    extern std::vector<uint8_t> _objectSelectionFlags;
    extern uint32_t _numSelectedObjectsForType[EnumValue(ObjectType::count)];

    void ObjectListLoad();
    ResultWithMessage CheckPark();
    std::pair<ObjectType, StringId> CheckObjectSelection();

    uint8_t GetSelectedObjectFlags(ObjectType objectType, size_t index);
    void ClearSelectedObject(ObjectType objectType, size_t index, uint32_t flags);
    void SetSelectedObject(ObjectType objectType, size_t index, uint32_t flags);

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

    StringId getStepStringId(Step step);
} // namespace OpenRCT2::Editor
